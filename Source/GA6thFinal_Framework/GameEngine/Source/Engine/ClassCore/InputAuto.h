#pragma once
namespace ReflectHelper
{
    // 구현부
    namespace ImGuiDraw
    {
        namespace Private
        {
            void EngineLog(int logLevel, std::string_view message,
                           std::source_location location = std::source_location::current());

            template <class T>
            bool InputAuto(T field, const InputAutoSetting& setting)
            {
                using namespace DirectX::SimpleMath;
                auto NotArrayTypeFunc = [&setting](auto* value, const char* name) {
                    using value_type          = std::remove_cvref_t<decltype(*value)>;
                    using OriginType          = std::remove_cvref_t<PropertyUtils::get_field_type_t<value_type>>;
                    constexpr bool isProperty = PropertyUtils::is_TProperty_v<value_type>;
                    constexpr bool isSetter   = PropertyUtils::is_setter<value_type>;
                    constexpr bool isLock     = isProperty == true && isSetter == false;

                    bool  isEdit = false;
                    bool  result = false;
                    auto& val    = *value;

                    if constexpr (isLock == true)
                    {
                        ImGui::BeginDisabled();
                    }

                    if constexpr (std::is_same_v<OriginType, int>)
                    {
                        int input = val;
                        isEdit    = ImGui::InputInt(name, &input, setting._int.step, setting._int.step_fast,
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
                        isEdit      = ImGui::InputFloat(name, &input, setting._float.step, setting._float.step_fast,
                                                        setting._float.format.c_str(), setting._float.flags);

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
                        isEdit =
                            ImGui::InputDouble(name, &input, InputAutoSetting::Double::step, setting._double.step_fast,
                                               setting._double.format.c_str(), setting._double.flags);

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
                        isEdit = ImGui::InputText(name, &input, setting._string.flags, setting._string.callback,
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
                        isEdit = ImGui::InputText(name, &input, setting._string.flags, setting._string.callback,
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
                        isEdit = ImGui::DragFloat2(name, &input.x, setting._Vector2.v_speed, setting._Vector2.v_min,
                                                   setting._Vector2.v_max, setting._Vector2.format.c_str(),
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
                        isEdit = ImGui::DragFloat3(name, &input.x, setting._Vector3.v_speed, setting._Vector3.v_min,
                                                   setting._Vector3.v_max, setting._Vector3.format.c_str(),
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
                        isEdit = ImGui::DragFloat4(name, &input.x, setting._Vector4.v_speed, setting._Vector4.v_min,
                                                   setting._Vector4.v_max, setting._Vector4.format.c_str(),
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
                        EngineLog(LogLevel::LEVEL_WARNING,
                                  std::format("{}, {} - {}", (const char*)u8"정의되지 않은 InputAuto 타입입니다.",
                                              typeid(val).name(), name)
                                      .c_str());
                    }
                    if constexpr (isLock == true)
                    {
                        ImGui::EndDisabled();
                    }
                    if constexpr (isProperty == true)
                    {
                        if (ImGui::BeginDragDropTarget())
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
                    using OriginType = PropertyUtils::get_field_type_t<value_type>;

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
                                int i = 0;
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

                if (isEdit)
                {
                    //UmCommandManager.Do<>();
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
} // namespace ReflectHelper