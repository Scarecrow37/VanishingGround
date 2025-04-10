#pragma once
#undef max
#undef min
#include <rfl.hpp>
#include <rfl/json.hpp>

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
    virtual void ImGuiDrawPropertys() {
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
        if (_reflectFields != nullptr)
        {
            free(_reflectFields);
            _reflectFields = nullptr;
        }
    }

private:
    void*              _reflectFields = nullptr;
    unsigned long long _fieldsSize    = 0;
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

#define REFLECT_FIELDS_BEGIN(BASE)                                                  \
    using Base = BASE;                                                              \
    struct reflect_fields_struct                                                    \
    {                                                                               \
        rfl::Flatten<Base::reflect_fields_struct> Basefields{};

#define REFLECT_FIELDS_END(CLASS)                                                   \
    }                                                                               \
    ;                                                                               \
    struct reflection_safe_ptr                                                      \
    {                                                                               \
        reflection_safe_ptr(CLASS##* owner)                                         \
        {                                                                           \
            _owner = owner;                                                         \
        }                                                                           \
        ~reflection_safe_ptr() = default;                                           \
        reflect_fields_struct* operator->()                                         \
        {                                                                           \
            return Get();                                                           \
        }                                                                           \
        reflect_fields_struct& operator*()                                          \
        {                                                                           \
            return *Get();                                                          \
        }                                                                           \
        reflect_fields_struct* Get()                                                \
        {                                                                           \
            if (_reflection == nullptr)                                             \
            {                                                                       \
                _reflection =                                                       \
                    reinterpret_cast<CLASS## ::reflect_fields_struct*>(             \
                        _owner->get_reflect_fields());                              \
            }                                                                       \
            return _reflection;                                                     \
        }                                                                           \
                                                                                    \
    private:                                                                        \
        CLASS## ::reflect_fields_struct* _reflection = nullptr;                     \
        CLASS##*                         _owner      = nullptr;                     \
    };                                                                              \
    reflection_safe_ptr ReflectionFields{this};                                     \
                                                                                    \
public:                                                                             \
    virtual std::string SerializedReflectFields()                                   \
    {                                                                               \
        serialized_reflect_event_recursive();                                       \
        return ReflectHelper::json::SerializedObjet(*ReflectionFields);             \
    }                                                                               \
    virtual bool DeserializedReflectFields(std::string_view data)                   \
    {                                                                               \
        bool result =                                                               \
            ReflectHelper::json::DeserializedObjet(*ReflectionFields, data);        \
        deserialized_reflect_event_recursive();                                     \
        return result;                                                              \
    }                                                                               \
                                                                                    \
protected:                                                                          \
    virtual void make_reflect_fields(void*& fields, unsigned long long& size)       \
    {                                                                               \
        size_t size_of = sizeof(CLASS## ::reflect_fields_struct);                   \
        fields         = malloc(size_of);                                           \
        size           = size_of;                                                   \
        new (fields) CLASS## ::reflect_fields_struct();                             \
    }                                                                               \
    virtual void serialized_reflect_event_recursive()                               \
    {                                                                               \
        Base::serialized_reflect_event_recursive();                                 \
        if constexpr (std::is_same_v<decltype(Base::SerializedReflectEvent),        \
                                     decltype(CLASS##::SerializedReflectEvent)>     \
                                     == false)                                      \
        {                                                                           \
            CLASS## ::SerializedReflectEvent();                                     \
        }                                                                           \
    }                                                                               \
    virtual void deserialized_reflect_event_recursive()                             \
    {                                                                               \
        Base::deserialized_reflect_event_recursive();                               \
        if constexpr (std::is_same_v<decltype(Base::DeserializedReflectEvent),      \
                                     decltype(CLASS##::DeserializedReflectEvent)>   \
                                     == false)                                      \
        {                                                                           \
            CLASS## ::DeserializedReflectEvent();                                   \
        }                                                                           \
    }

// 에디터 편집을 허용할 프로퍼티들을 등록합니다. Get, Set 함수가 모두 존재하는
// 프로퍼티만 편집 가능합니다.
#define REFLECT_PROPERTY(...)                                                  \
    virtual void ImGuiDrawPropertys()                                          \
    {                                                                          \
        __super::ImGuiDrawPropertys();                                         \
        auto                             fields = std::tie(__VA_ARGS__);       \
        static std::unordered_set<void*> reflectionFieldsSet;                  \
        reflectionFieldsSet.clear();                                           \
                                                                               \
        ImGui::PushID(this);                                                   \
        StdHelper::for_each_tuple(fields, [&](auto& field) {                   \
            using FieldType = std::remove_cvref_t<decltype(field)>;            \
            if constexpr (PropertyUtils::is_TProperty_v<FieldType>)            \
            {                                                                  \
                static_assert(FieldType::is_getter,                            \
                              "This property does not have a getter.");        \
                static_assert(FieldType::is_setter,                            \
                              "This property does not have a setter.");        \
                ReflectHelper::ImGuiDraw::Private::InputAuto(field);           \
            }                                                                  \
            else                                                               \
            {                                                                  \
                reflectionFieldsSet.insert(&field);                            \
            }                                                                  \
        });                                                                    \
        const auto view = rfl::to_view(*ReflectionFields.Get());               \
        view.apply([&](auto& rflField) {                                       \
            if (reflectionFieldsSet.find(rflField.value()) !=                  \
                reflectionFieldsSet.end())                                     \
            {                                                                  \
                ReflectHelper::ImGuiDraw::Private::InputAuto(rflField);        \
            }                                                                  \
        });                                                                    \
        ImGui::PopID();                                                        \
    }

namespace ReflectHelper
{
    namespace ImGuiDraw
    {
        struct InputAutoSetting
        {
            struct Int
            {
                static int                 step;
                static int                 step_fast;
                static ImGuiInputTextFlags flags;
            };

            struct Float
            {
                static float               step;
                static float               step_fast;
                static std::string         format;
                static ImGuiInputTextFlags flags;
            };

            struct Double
            {
                static double              step;
                static double              step_fast;
                static std::string         format;
                static ImGuiInputTextFlags flags;
            };

            struct String
            {
                static ImGuiInputTextFlags    flags;
                static ImGuiInputTextCallback callback;
                static void*                  user_data;
            };

            struct Vector2
            {
                static float            v_speed;
                static float            v_min;
                static float            v_max;
                static std::string      format;
                static ImGuiSliderFlags flags;
            };

            struct Vector3
            {
                static float            v_speed;
                static float            v_min;
                static float            v_max;
                static std::string      format;
                static ImGuiSliderFlags flags;
            };

            struct Vector4
            {
                static float            v_speed;
                static float            v_min;
                static float            v_max;
                static std::string      format;
                static ImGuiSliderFlags flags;
            };
        };

        template <class T>
        void InputReflectFields(T& obj);

        template <class T>
        void InputReflectFields(T* obj);
    } // namespace ImGuiDraw

    // serialized helper
    namespace json
    {
        template <typename Type>
        std::string SerializedObjet(Type& obj);

        template <typename Type>
        bool DeserializedObjet(Type& obj, std::string_view data);
    } // namespace json
} // namespace ReflectHelper

namespace ReflectHelper
{
    // 구현부
    namespace ImGuiDraw
    {
        namespace Private
        {
            void EngineLog(int logLevel, std::string_view message,
                           std::source_location location =
                               std::source_location::current());
            template <class T>
            bool InputAuto(T field)
            {
                using namespace DirectX::SimpleMath;
                auto NotArrayTypeFunc = [](auto* value, const char* name) {
                    using value_type = std::remove_cvref_t<decltype(*value)>;
                    using OriginType = std::remove_cvref_t<
                        PropertyUtils::get_field_type_t<value_type>>;
                    constexpr bool isProperty =
                        PropertyUtils::is_TProperty_v<value_type>;

                    bool  isEdit = false;
                    auto& val    = *value;

                    if constexpr (std::is_same_v<OriginType, int>)
                    {
                        int input = val;
                        isEdit    = ImGui::InputInt(
                            name, &input, InputAutoSetting::Int::step,
                            InputAutoSetting::Int::step_fast,
                            InputAutoSetting::Int::flags);

                        if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                        {
                            val = input;
                        }
                    }
                    else if constexpr (std::is_same_v<OriginType, float>)
                    {
                        float input = val;
                        isEdit      = ImGui::InputFloat(
                            name, &input, InputAutoSetting::Float::step,
                            InputAutoSetting::Float::step_fast,
                            InputAutoSetting::Float::format.c_str(),
                            InputAutoSetting::Float::flags);

                        if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                        {
                            val = input;
                        }
                    }
                    else if constexpr (std::is_same_v<OriginType, double>)
                    {
                        double input = val;
                        isEdit       = ImGui::InputDouble(
                            name, &input, InputAutoSetting::Double::step,
                            InputAutoSetting::Double::step_fast,
                            InputAutoSetting::Double::format.c_str(),
                            InputAutoSetting::Double::flags);

                        if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                        {
                            val = input;
                        }
                    }
                    else if constexpr (std::is_same_v<OriginType, bool>)
                    {
                        bool input = val;
                        isEdit     = ImGui::Checkbox(name, &input);

                        if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                        {
                            val = input;
                        }
                    }
                    else if constexpr (std::is_same_v<OriginType, std::string>)
                    {
                        static std::string input;
                        input  = val;
                        isEdit = ImGui::InputText(
                            name, &input, InputAutoSetting::String::flags,
                            InputAutoSetting::String::callback,
                            InputAutoSetting::String::user_data);

                        if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                        {
                            val = input;
                        }
                    }
                    else if constexpr (isProperty &&
                                       std::is_same_v<OriginType,
                                                      std::string_view>)
                    {
                        static std::string input;
                        input  = val;
                        isEdit = ImGui::InputText(
                            name, &input, InputAutoSetting::String::flags,
                            InputAutoSetting::String::callback,
                            InputAutoSetting::String::user_data);

                        if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                        {
                            val = input;
                        }
                    }
                    else if constexpr (isProperty &&
                                       std::is_same_v<
                                           OriginType,
                                           DirectX::SimpleMath::Vector2>)
                    {
                        DirectX::SimpleMath::Vector2 input = val;
                        isEdit                             = ImGui::DragFloat2(
                            name, &input.x, InputAutoSetting::Vector2::v_speed,
                            InputAutoSetting::Vector2::v_min,
                            InputAutoSetting::Vector2::v_max,
                            InputAutoSetting::Vector2::format.c_str(),
                            InputAutoSetting::Vector2::flags);

                        if (isEdit)
                        {
                            val = input;
                        }
                    }
                    else if constexpr (isProperty &&
                                       std::is_same_v<
                                           OriginType,
                                           DirectX::SimpleMath::Vector3>)
                    {
                        DirectX::SimpleMath::Vector3 input = val;
                        isEdit                             = ImGui::DragFloat3(
                            name, &input.x, InputAutoSetting::Vector3::v_speed,
                            InputAutoSetting::Vector3::v_min,
                            InputAutoSetting::Vector3::v_max,
                            InputAutoSetting::Vector3::format.c_str(),
                            InputAutoSetting::Vector3::flags);

                        if (isEdit)
                        {
                            val = input;
                        }
                    }
                    else if constexpr (isProperty &&
                                       std::is_same_v<
                                           OriginType,
                                           DirectX::SimpleMath::Vector4>)
                    {
                        DirectX::SimpleMath::Vector4 input = val;
                        isEdit                             = ImGui::DragFloat4(
                            name, &input.x, InputAutoSetting::Vector4::v_speed,
                            InputAutoSetting::Vector4::v_min,
                            InputAutoSetting::Vector4::v_max,
                            InputAutoSetting::Vector4::format.c_str(),
                            InputAutoSetting::Vector4::flags);

                        if (isEdit)
                        {
                            val = input;
                        }
                    }
                    else
                    {
                        assert(!"처리되지 않는 InputAuto 타입입니다.");
                        EngineLog(
                            LogLevel::LEVEL_WARNING,
                            std::format("{}, {}", typeid(val).name(), name)
                                .c_str());
                    }

                    return isEdit;
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
                    bool isEdit = false;
                    if constexpr (StdHelper::is_std_array_v<OriginType>)
                    {
                        if (ImGui::CollapsingHeader((const char*)name.data()))
                        {
                            if constexpr (std::ranges::range<decltype(*value)>)
                            {
                                int i = 0;
                                for (auto& val : *value)
                                {
                                    isEdit = NotArrayTypeFunc(
                                        &val, std::format("[{}]", i).c_str());
                                    i++;
                                }
                            }
                        }
                    }
                    else if constexpr (StdHelper::is_std_vector_v<OriginType>)
                    {
                        if constexpr (std::ranges::range<decltype(*value)>)
                        {
                            if (ImGui::CollapsingHeader(
                                    (const char*)name.data()))
                            {
                                bool isEdit = false;
                                int  i      = 0;
                                for (auto& val : *value)
                                {
                                    isEdit = NotArrayTypeFunc(
                                        &val, std::format("[{}]", i).c_str());
                                    i++;
                                }
                                if (ImGui::Button("+"))
                                {
                                    value->emplace_back();
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("-"))
                                {
                                    value->pop_back();
                                }
                            }
                        }
                    }
                    else
                    {
                        isEdit = NotArrayTypeFunc(value, name.data());
                    }
                }
                ImGui::PopID();
                return isEdit;
            }
        } // namespace Private

        template <class T>
        void InputReflectFields(T& obj)
        {
            const auto view = rfl::to_view(obj);
            view.apply([](auto& f) { ImGuiDraw::Private::InputAuto(f); });
        }

        template <class T>
        void InputReflectFields(T* obj)
        {
            const auto view = rfl::to_view(*obj);
            view.apply([](auto& f) { ImGuiDraw::Private::InputAuto(f); });
        }
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
                assert(!"역직렬화 실패.");
            }
            return true;
        }
    } // namespace json
} // namespace ReflectHelper