#pragma once
#undef max
#undef min
#include <rfl/json.hpp>
#include <rfl.hpp>

//reflect-cpp 라이브러리 docs https://rfl.getml.com/docs-readme/#the-basics
//reflect-cpp github https://github.com/getml/reflect-cpp
                                                                  
//자동 직렬화 사용하기 위한 클래스
struct ReflectSerializer : 
    public IReflectProperty
{
    ReflectSerializer() = default;
    virtual ~ReflectSerializer()
    {
        if (_reflectFields != nullptr)
        {
            free(_reflectFields);
            _reflectFields = nullptr;
        }
    }
    
protected:
    /*직렬화 직전 호출되는 이벤트 함수입니다.*/
    virtual void SerializedReflectEvent() {}
    /*역직렬화 이후 호출되는 이벤트 입니다.*/
    virtual void DeserializedReflectEvent() {}
private:
    void*              _reflectFields = nullptr;
    unsigned long long _fieldsSize = 0;

#pragma region 매크로가 생성하는 가상함수들.
public:
    virtual std::string serialized_reflect_fields()
    {
        assert(!"REFLECT_FIELDS가 정의되지 않았습니다.");
        return "{}";
    }
    virtual bool deserialized_reflect_fields(std::string_view data)
    {
        assert(!"REFLECT_FIELDS가 정의되지 않았습니다.");
        return false;
    }
protected:
    struct reflect_fields_struct {};
    virtual void make_reflect_fields(void*& fields, unsigned long long& size)
    {
        assert(!"REFLECT_FIELDS가 정의되지 않았습니다.");
        size_t size_of = sizeof(ReflectSerializer::reflect_fields_struct);
        fields = malloc(size_of);
        new(fields)ReflectSerializer::reflect_fields_struct();
        size = size_of;
    }
    virtual void serialized_reflect_event_recursive() { SerializedReflectEvent(); }
    virtual void deserialized_reflect_event_recursive() { DeserializedReflectEvent(); }
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
                                                                 
#define REFLECT_FIELDS_BEGIN(BASE)							                                                            \
using Base = BASE;										                                                                \
struct reflect_fields_struct                                            	                                            \
{														                                                                \
	rfl::Flatten<Base::reflect_fields_struct> Basefields{};	                         
                                                                             
#define REFLECT_FIELDS_END(CLASS)	                                                                                    \
};                                                                                                                      \
struct reflection_safe_ptr                                                                                              \
{                                                                                                                       \
    reflection_safe_ptr(CLASS##* owner)                                                                                 \
    {                                                                                                                   \
        _owner = owner;                                                                                                 \
    }                                                                                                                   \
    ~reflection_safe_ptr() = default;                                                                                   \
    CLASS##::reflect_fields_struct* operator->()                                                                        \
    {                                                                                                                   \
        return Get();                                                                                                   \
    }                                                                                                                   \
    CLASS##::reflect_fields_struct* Get()                                                                               \
    {                                                                                                                   \
        if (_reflection == nullptr)                                                                                     \
        {                                                                                                               \
            _reflection = reinterpret_cast<CLASS##::reflect_fields_struct*>(_owner->get_reflect_fields());              \
        }                                                                                                               \
        return _reflection;                                                                                             \
    }                                                                                                                   \
private:                                                                                                                \
    CLASS##::reflect_fields_struct* _reflection = nullptr;                                                              \
    CLASS##* _owner = nullptr;                                                                                          \
};                                                                                                                      \
reflection_safe_ptr ReflectionFields{ this };                                                                           \
public:                                                                                                                 \
virtual std::string serialized_reflect_fields()                                                                         \
{                                                                                                                       \
    serialized_reflect_event_recursive();                                                                               \
    return ReflectHelper::json::SerializedObjet(*ReflectionFields.Get());                                               \
}                                                                                                                       \
virtual bool deserialized_reflect_fields(std::string_view data)                                                         \
{                                                                                                                       \
    bool result = ReflectHelper::json::DeserializedObjet(*ReflectionFields.Get(), data);                                \
    deserialized_reflect_event_recursive();                                                                             \
    return result;                                                                                                      \
}                                                                                                                       \
protected:                                                                                                              \
virtual void make_reflect_fields(void*& fields, unsigned long long& size)                                               \
{                                                                                                                       \
    size_t size_of = sizeof(CLASS##::reflect_fields_struct);                                                            \
    fields = malloc(size_of);                                                                                           \
    size = size_of;                                                                                                     \
    new(fields)CLASS##::reflect_fields_struct();                                                                        \
}                                                                                                                       \
virtual void serialized_reflect_event_recursive()                                                                       \
{                                                                                                                       \
    Base::SerializedReflectEvent();                                                                                     \
    Base::serialized_reflect_event_recursive();                                                                         \
}                                                                                                                       \
virtual void deserialized_reflect_event_recursive()                                                                     \
{                                                                                                                       \
    Base::DeserializedReflectEvent();                                                                                   \
    Base::deserialized_reflect_event_recursive();                                                                       \
}

namespace ReflectHelper
{
    namespace ImGuiDraw
    {
        namespace InputAutoSetting
        {
            struct InputAutoSettingInt
            {
                static int step;
                static int step_fast;
                static ImGuiInputTextFlags flags;
            };
            extern InputAutoSettingInt Int;

            struct InputAutoSettingFloat
            {
                static float step;
                static float step_fast;
                static std::string format;
                static ImGuiInputTextFlags flags;
            };
            extern InputAutoSettingFloat Float;

            struct InputAutoSettingDouble
            {
                static double step;
                static double step_fast;
                static std::string format;
                static ImGuiInputTextFlags flags;
            };
            extern InputAutoSettingDouble Double;

            struct InputAutoSettingString
            {
                static ImGuiInputTextFlags flags;
                static ImGuiInputTextCallback callback;
                static void* user_data;
            };
            extern InputAutoSettingString String;

            struct InputAutoSettingVector3
            {
                static ImGuiInputTextFlags falgs;
            };
            extern InputAutoSettingVector3 vector3;
        };

        template<class T>
        void InputReflectFields(T& obj);

        template<class T>
        void InputReflectFields(T* obj);
    }

    //serialized helper
    namespace json
    {
        template<typename Type>
        std::string SerializedObjet(Type& obj);

        template<typename Type>
        bool DeserializedObjet(Type& obj, std::string_view data);
    }
}








namespace ReflectHelper
{
    //구현부
    namespace ImGuiDraw
    {
        namespace Private
        {
            template<class T>
            bool InputAuto(T field)
            {
                using namespace DirectX::SimpleMath;
                auto NotArrayTypeFunc =
                    [](auto* value, const char* name)
                    {
                        using namespace InputAutoSetting;
                        using value_type = std::remove_cvref_t<decltype(*value)>;
                        using OriginType = PropertyUtils::get_field_type_t<value_type>;
                        constexpr bool isProperty = PropertyUtils::is_TProperty_v<value_type>;

                        bool isEdit = false;
                        auto& val = *value;

                        if constexpr (std::is_same_v<OriginType, int>)
                        {
                            int input = val;
                            isEdit = ImGui::InputInt(name, &input,
                                Int.step,
                                Int.step_fast,
                                Int.flags);

                            if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                            {
                                val = input;
                            }
                        }
                        else if constexpr (std::is_same_v<OriginType, float>)
                        {
                            float input = val;
                            isEdit = ImGui::InputFloat(name, &input,
                                Float.step,
                                Float.step_fast,
                                Float.format.c_str(),
                                Float.flags);

                            if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                            {
                                val = input;
                            }
                        }
                        else if constexpr (std::is_same_v<OriginType, double>)
                        {
                            double input = val;
                            isEdit = ImGui::InputDouble(name, &input,
                                Float.step,
                                Float.step_fast,
                                Float.format.c_str(),
                                Float.flags);

                            if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                            {
                                val = input;
                            }
                        }
                        else if constexpr (std::is_same_v<OriginType, bool>)
                        {
                            bool input = val;
                            isEdit = ImGui::Checkbox(name, &input);

                            if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                            {
                                val = input;
                            }
                        }
                        else if constexpr (std::is_same_v<OriginType, std::string>)
                        {
                            static std::string input;
                            input = val;
                            isEdit = ImGui::InputText(name, &input,
                                String.flags,
                                String.callback,
                                String.user_data);

                            if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                            {
                                val = input;
                            }
                        }
                        else if constexpr (isProperty && std::is_same_v<OriginType, std::string_view>)
                        {
                            static std::string input;
                            input = val;
                            isEdit = ImGui::InputText(name, &input,
                                String.flags,
                                String.callback,
                                String.user_data);

                            if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                            {
                                val = input;
                            }
                        }
                        else if constexpr (isProperty && std::is_same_v<OriginType, DirectX::SimpleMath::Vector3>)
                        {
                            DirectX::SimpleMath::Vector3 input = val;
                            isEdit = ImGui::InputFloat3(
                                "Vector3", 
                                &input, 
                                vector3.falgs);

                            if (isEdit && ImGui::IsItemDeactivatedAfterEdit())
                            {
                                val = input;
                            }
                        }

                        return isEdit;
                    };

                bool isEdit = false;
                ImGui::PushID(typeid(T).hash_code());
                {
                    const type_info& type = typeid(*field.value());
                    const auto& name = field.name();
                    auto* value = field.value();

                    using value_type = std::remove_cvref_t<decltype(*value)>;
                    using OriginType = PropertyUtils::get_field_type_t<value_type>;
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
                                bool isEdit = false;
                                int i = 0;
                                for (auto& val : *value)
                                {
                                    isEdit = NotArrayTypeFunc(&val, std::format("[{}]", i).c_str());
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
        }

        template<class T>
        void InputReflectFields(T& obj)
        {
            const auto view = rfl::to_view(obj);
            view.apply([](auto& f)
                {
                    ImGuiDraw::Private::InputAuto(f);
                });
        }

        template<class T>
        void InputReflectFields(T* obj)
        {
            const auto view = rfl::to_view(*obj);
            view.apply([](auto& f)
                {
                    ImGuiDraw::Private::InputAuto(f);
                });
        }
    }


    namespace json
    {
        template<typename Type>
        inline std::string SerializedObjet(Type& obj)
        {
            return rfl::json::write(obj);
        }

        template<typename Type>
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
    }
}