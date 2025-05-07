#pragma once
#undef max
#undef min
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <rfl/yaml.hpp>

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

// 자동 직렬화 및 REFLECT_PROPERTY를 사용하기 위한 클래스
struct ReflectSerializer
{
protected:
    /*
    직렬화 직전 자동으로 호출되는 이벤트 함수입니다.
    직접 override 해서 사용합니다.
    */
    virtual void SerializedReflectEvent() {}
    /*
    역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.
    직접 override 해서 사용합니다.
    */
    virtual void DeserializedReflectEvent() {}

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
    virtual std::string SerializedReflectFields()                                                               \
    {                                                                                                           \
        serialized_reflect_event_recursive();                                                                   \
        return ReflectHelper::json::SerializedObjet(*ReflectFields);                                            \
    }                                                                                                           \
    virtual bool DeserializedReflectFields(std::string_view data)                                               \
    {                                                                                                           \
        bool result =                                                                                           \
            ReflectHelper::json::DeserializedObjet(*ReflectFields, data);                                       \
        deserialized_reflect_event_recursive();                                                                 \
        return result;                                                                                          \
    }                                                                                                           \
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
    virtual void applyReflectFields(const std::function<void(std::string_view, void*)>& func)                   \
    {                                                                                                           \
        const auto view = rfl::to_view(*ReflectFields.Get());                                                   \
        view.apply([&](auto& rflField)                                                                          \
        {                                                                                                       \
            func(rflField.name(), rflField.value());                                                            \
        });                                                                                                     \
    }                                                                                                           \
                       
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
        const auto view = rfl::to_view(*ReflectFields.Get());                                               \
        view.apply([&](auto& rflField) {                                                                    \
            if (reflectionFieldsSet.find(rflField.value()) !=                                               \
                reflectionFieldsSet.end())                                                                  \
            {                                                                                               \
                ReflectHelper::ImGuiDraw::Private::InputAuto(rflField, setting);                            \
            }                                                                                               \
        });                                                                                                 \
        if (isTail)                                                                                         \
        {                                                                                                   \
            setting.InputEndEvent = nullptr;                                                                \
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
                int                 step      = 0;
                int                 step_fast = 0;
                ImGuiInputTextFlags flags     = 0;
            }
            _int;

            struct Float
            {
                float               step      = 0;
                float               step_fast = 0;
                std::string         format    = "%.3f";
                ImGuiInputTextFlags flags     = 0;
            }
            _float;

            struct Double
            {
                double              step      = 0;
                double              step_fast = 0;
                std::string         format    = "%.6f";
                ImGuiInputTextFlags flags     = 0;
            }
            _double;

            struct String
            {
                ImGuiInputTextFlags    flags     = 0;
                ImGuiInputTextCallback callback  = nullptr;
                void*                  user_data = nullptr;
            }
            _string;

            struct Vector2
            {
                float            v_speed = 0.1f;
                float            v_min   = 0.f;
                float            v_max   = 0.f;
                std::string      format  = "%.3f";
                ImGuiSliderFlags flags   = 0;
            }
            _Vector2;

            struct Vector3
            {
                float            v_speed = 0.1f;
                float            v_min   = 0.f;
                float            v_max   = 0.f;
                std::string      format  = "%.3f";
                ImGuiSliderFlags flags   = 0;
            }
            _Vector3;

            struct Vector4
            {
                float            v_speed = 0.1f;
                float            v_min   = 0.f;
                float            v_max   = 0.f;
                std::string      format  = "%.3f";
                ImGuiSliderFlags flags   = 0;
            }
            _Vector4;

            //InputAuto return 직전에 호출해주는 함수객체 입니다. 
            //매개변수로 Input 여부랑 접근한 맴버의 이름을 전달해줍니다.
            //ImGuiDrawPropertys() 함수 호출 이후 NULL로 초기화됩니다.
            std::function<void(bool, std::string_view)> InputEndEvent;
        };
    } // namespace ImGuiDraw

    // serialized helper
    namespace json
    {
        template <typename Type>
        std::string SerializedObjet(Type& obj);

        template <typename Type>
        bool DeserializedObjet(Type& obj, std::string_view data);

        /// <summary>
        /// <para>전달한 yyjson_val*를 char*형식의 문자열로 변환해서 반환합니다.</para>
        /// <para>반드시 반환된 값이 nullptr이 아니면 사용후 free로 해제해야 합니다.</para>
        /// </summary>
        /// <param name="val :">변환할 val 값</param>
        /// <returns>변환된 문자열</returns>
        char* yyjsonValToCStr(yyjson_val* val);
    } // namespace json
} // namespace ReflectHelper

namespace ReflectHelper
{
    // 구현부
    namespace ImGuiDraw
    {
        namespace Private
        {
            void EngineLog(int logLevel, std::string_view message, std::source_location location = std::source_location::current());

            template <class T>
            bool InputAuto(T field, const InputAutoSetting& setting)
            {
                using namespace DirectX::SimpleMath;
                auto NotArrayTypeFunc = [&setting](auto* value, const char* name) {
                    using value_type = std::remove_cvref_t<decltype(*value)>;
                    using OriginType = std::remove_cvref_t<
                        PropertyUtils::get_field_type_t<value_type>>;
                    constexpr bool isProperty =
                        PropertyUtils::is_TProperty_v<value_type>;
                    constexpr bool isSetter = PropertyUtils::is_setter<value_type>;
                    constexpr bool isLock = isProperty == true && isSetter == false;

                    bool isEdit = false;
                    bool result = false;
                    auto& val    = *value;

                    if constexpr (isLock == true)
                    {
                        ImGui::BeginDisabled();
                    }

                    if constexpr (std::is_same_v<OriginType, int>)
                    {
                        int input = val;
                        isEdit    = ImGui::InputInt(name, &input, 
                            setting._int.step,
                            setting._int.step_fast,
                            setting._int.flags);

                        if constexpr (isProperty == false || isSetter == true)
                        {
                            if (isEdit)
                            {
                                val = input;
                            }
                            if (ImGui::IsItemDeactivatedAfterEdit())
                            {
                                result = true;
                            }
                        }
                    }
                    else if constexpr (std::is_same_v<OriginType, float>)
                    {
                        float input = val;
                        isEdit      = ImGui::InputFloat(
                            name, &input, 
                            setting._float.step,
                            setting._float.step_fast,
                            setting._float.format.c_str(),
                            setting._float.flags);

                        if constexpr (isProperty == false || isSetter == true)
                        {
                            if (isEdit)
                            {
                                val = input;
                            }
                            if (ImGui::IsItemDeactivatedAfterEdit())
                            {
                                result = true;
                            }
                        }
                    }
                    else if constexpr (std::is_same_v<OriginType, double>)
                    {
                        double input = val;
                        isEdit       = ImGui::InputDouble(
                            name, &input, InputAutoSetting::Double::step,
                            setting._double.step_fast,
                            setting._double.format.c_str(),
                            setting._double.flags);

                        if constexpr (isProperty == false || isSetter == true)
                        {
                            if (isEdit)
                            {
                                val = input;
                            }
                            if (ImGui::IsItemDeactivatedAfterEdit())
                            {
                                result = true;
                            }
                        }                      
                    }
                    else if constexpr (std::is_same_v<OriginType, bool>)
                    {
                        bool input = val;
                        isEdit     = ImGui::Checkbox(name, &input);

                        if constexpr (isProperty == false || isSetter == true)
                        {
                            if (isEdit)
                            {
                                val = input;
                            }
                            if (ImGui::IsItemDeactivatedAfterEdit())
                            {
                                result = true;
                            }
                        }
                    }
                    else if constexpr (std::is_same_v<OriginType, std::string>)
                    {
                        static std::string input;
                        input  = val;
                        isEdit = ImGui::InputText(
                            name, &input, 
                            setting._string.flags,
                            setting._string.callback,
                            setting._string.user_data);

                        if constexpr (isProperty == false || isSetter == true)
                        {
                            if (isEdit)
                            {
                                val = input;
                            }
                            if (ImGui::IsItemDeactivatedAfterEdit())
                            {
                                result = true;
                            }
                        }
                    }
                    else if constexpr (isProperty && std::is_same_v<OriginType, std::string_view>)
                    {
                        static std::string input;
                        input  = val;
                        isEdit = ImGui::InputText(
                            name, 
                            &input, 
                            setting._string.flags,
                            setting._string.callback,
                            setting._string.user_data);

                        if constexpr (isProperty == false || isSetter == true)
                        {
                            if (isEdit)
                            {
                                val = input;                               
                            }
                            if (ImGui::IsItemDeactivatedAfterEdit())
                            {
                                result = true;
                            }
                        }
                    }
                    else if constexpr (isProperty && std::is_same_v<OriginType, DirectX::SimpleMath::Vector2>)
                    {
                        DirectX::SimpleMath::Vector2 input = val;
                        isEdit = ImGui::DragFloat2(
                            name, 
                            &input.x, 
                            setting._Vector2.v_speed,
                            setting._Vector2.v_min,
                            setting._Vector2.v_max,
                            setting._Vector2.format.c_str(),
                            setting._Vector2.flags);

                        if constexpr (isProperty == false || isSetter == true)
                        {
                            if (isEdit)
                            {
                                val = input;
                            }
                            if (ImGui::IsItemDeactivatedAfterEdit())
                            {
                                result = true;
                            }
                        }
                    }
                    else if constexpr (isProperty && std::is_same_v<OriginType, DirectX::SimpleMath::Vector3>)
                    {
                        DirectX::SimpleMath::Vector3 input = val;
                        isEdit = ImGui::DragFloat3(
                            name, 
                            &input.x, 
                            setting._Vector3.v_speed,
                            setting._Vector3.v_min,
                            setting._Vector3.v_max,
                            setting._Vector3.format.c_str(),
                            setting._Vector3.flags);

                        if constexpr (isProperty == false || isSetter == true)
                        {
                            if (isEdit)
                            {
                                val = input;
                            }
                            if (ImGui::IsItemDeactivatedAfterEdit())
                            {
                                result = true;
                            }
                        }
                    }
                    else if constexpr (isProperty && std::is_same_v<OriginType, DirectX::SimpleMath::Vector4>)
                    {
                        DirectX::SimpleMath::Vector4 input = val;
                        isEdit = ImGui::DragFloat4(
                            name, 
                            &input.x, 
                            setting._Vector4.v_speed,
                            setting._Vector4.v_min,
                            setting._Vector4.v_max,
                            setting._Vector4.format.c_str(),
                            setting._Vector4.flags);

                        if constexpr (isProperty == false || isSetter == true)
                        {
                            if (isEdit)
                            {
                                val = input;
                            }
                            if (ImGui::IsItemDeactivatedAfterEdit())
                            {
                                result = true;
                            }
                        }
                    }
                    else
                    {
                        EngineLog(
                            LogLevel::LEVEL_WARNING,
                            std::format(
                                "{}, {} - {}", 
                                (const char*)u8"정의되지 않은 InputAuto 타입입니다.",
                                typeid(val).name(), 
                                name
                                )
                                .c_str());
                    }
                    if constexpr (isLock == true)
                    {
                        ImGui::EndDisabled();
                    }
                    if constexpr (isProperty == true)
                    {
                        if(ImGui::BeginDragDropTarget())
                        {
                            val.InvokeDragDropFunc();
                            ImGui::EndDragDropTarget();
                        }                   
                    }
                    return result;
                };

                bool isEdit = false;
                ImGui::PushID(field.value());
                {
                    const type_info& type  = typeid(*field.value());
                    const auto&      name  = field.name();
                    auto*            value = field.value();

                    using value_type = std::remove_cvref_t<decltype(*value)>;
                    using OriginType =
                        PropertyUtils::get_field_type_t<value_type>;
                    if constexpr (StdHelper::is_std_array_v<OriginType>)
                    {
                        if (ImGui::CollapsingHeader((const char*)name.data()))
                        {
                            if constexpr (std::ranges::range<decltype(*value)>)
                            {
                                int i = 0;
                                for (auto& val : *value)
                                {
                                    isEdit = NotArrayTypeFunc(&val, std::format("[{}]", i).c_str());
                                    i++;
                                }
                            }
                        }
                    }
                    else if constexpr (StdHelper::is_std_vector_v<OriginType>)
                    {
                        if constexpr (std::ranges::range<decltype(*value)>)
                        {
                            if (ImGui::CollapsingHeader((const char*)name.data()))
                            {
                                int  i = 0;
                                for (auto& val : *value)
                                {
                                    isEdit = NotArrayTypeFunc(&val, std::format("[{}]", i).c_str());
                                    i++;
                                }
                                if (ImGui::Button("+"))
                                {
                                    value->emplace_back();
                                    isEdit = true;
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("-"))
                                {
                                    if (value->size() > 0)
                                    {
                                        value->pop_back();
                                        isEdit = true;
                                    }                                 
                                }
                            }
                        }
                    }
                    else
                    {
                        isEdit = NotArrayTypeFunc(value, name.data());
                    }
                }

                if (setting.InputEndEvent)
                {
                    setting.InputEndEvent(isEdit, field.name());
                }
                ImGui::PopID();
                return isEdit;
            }
        } // namespace Private
    } // namespace ImGuiDraw

    namespace json
    {
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
                return false;
            }
            return true;
        }
    } // namespace json
} // namespace ReflectHelper