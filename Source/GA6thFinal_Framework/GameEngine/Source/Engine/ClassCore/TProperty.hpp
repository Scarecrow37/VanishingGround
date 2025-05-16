//프로퍼티 사용시 1회 포함
#define USING_PROPERTY(class_name)                                                             \
using property_class_type = class_name;                                                                           

#define GETTER(type, property_name)                                                            \
struct property_name##_property_getter_struct                                                  \
{                                                                                              \
    using Type = type;                                                                         \
    static constexpr const char* name = #property_name;                                        \
    inline type operator()(property_class_type* _this) const                                   \
    {                                                                                          \
        return _this->property_name##_property_getter();                                       \
    }                                                                                          \
};                                                                                             \
type property_name##_property_getter()                                         
                                                                                               
#define SETTER(type, property_name)                                                            \
struct property_name##_property_setter_struct                                                  \
{                                                                                              \
    using Type = type;                                                                         \
    static constexpr const char* name = #property_name;                                        \
    inline void operator()(property_class_type* _this, const std::remove_cvref_t<type>& value) \
    {                                                                                          \
        _this->property_name##_property_setter(value);                                         \
    }                                                                                          \
};                                                                                             \
void property_name##_property_setter(const std::remove_cvref_t<type>& value)

#define GETTER_ONLY(type, property_name)                                                       \
using property_name##_property_setter_struct = property_void_type;                             \
GETTER(type, property_name)               

#define SETTER_ONLY(type, property_name)                                                       \
using property_name##_property_getter_struct = property_void_type;                             \
SETTER(type, property_name)   

#define PROPERTY(property_name)                                                                \
TProperty<property_class_type, property_name##_property_getter_struct, property_name##_property_setter_struct> property_name{this};                \
using property_name##_property_t = TProperty<property_class_type, property_name##_property_getter_struct, property_name##_property_setter_struct>;                                                                                                               
                                                                                                                            
struct property_void_type
{
    using Type = void;
};

template <typename owner, class getter, class setter>
class TProperty
{
public:
    using owner_type = owner;
    static constexpr bool is_getter = !std::is_same_v<getter::Type, void>;
    static constexpr bool is_setter = !std::is_same_v<setter::Type, void>;

    /// <summary>
    /// <para> 이 프로퍼티의 에디터 창에서 AcceptDragDropPayload를 호출하는 함수를 설정합니다. </para>
    /// <para> 프로퍼티를 맴버로 사용하는 클래스의 생성자에서 사용해야합니다. </para>
    /// <para> if (ImGui::BeginDragDropTarget())후 호출해줍니다. </para>
    /// </summary>
    inline void SetDragDropFunc(const std::function<void()>& func) 
    {
        if (dragDropTargetFunc)
        {
            assert(!"이미 설정된 함수입니다.");
        }
        else
        {
            dragDropTargetFunc = func;
        }
    }

    inline void InvokeDragDropFunc()
    {
        if (dragDropTargetFunc)
        {
            dragDropTargetFunc();
        }
    }

    inline owner_type* GetOwner()
    {
        return _propertyOwner;
    }
private:
    static_assert(is_getter || is_setter, "TProperty must have either a getter or a setter.");

    static constexpr bool is_same_type_get_set = std::is_same_v<getter::Type, setter::Type>;
    static constexpr bool is_only = is_getter != is_setter;
    static_assert(is_only || is_same_type_get_set, "The getter and setter have different types.");

    using getterType = std::conditional_t<is_getter, getter, char>;
    using setterType = std::conditional_t<is_setter, setter, char>;
public:
    using field_type = std::conditional_t<is_getter, typename getter::Type, typename setter::Type>;
    using remove_cvref_field_type = std::remove_cvref_t<field_type>;
    static constexpr bool is_ref_getter = !std::is_pointer_v<field_type> && std::is_reference_v<field_type> && is_getter;

    TProperty(
        owner_type* _this
    ) 
        :
        _propertyOwner(_this),
        type_id(typeid(field_type))
    {
        
    }

private:
    owner_type* _propertyOwner = nullptr;
    getterType _getter{};
    setterType _setter{};
    const type_info& type_id;
    std::function<void()> dragDropTargetFunc;

    field_type Getter() const requires(is_getter)
    {
        return _getter(_propertyOwner);
    }
    void Setter(const remove_cvref_field_type& rhs) requires(is_setter)
    {
        _setter(_propertyOwner, rhs);
    }

public:
    //프로퍼티가 사용하는 field name
    inline static constexpr std::string_view name()
    {
        if constexpr (is_getter)
        {
            return std::string_view(getterType::name);
        }
        else
        {
            return std::string_view(setterType::name);
        } 
    }
    //프로퍼티가 사용하는 field typeid
    inline const type_info& type() const
    {
        return type_id;
    }
    //프로퍼티의 ptr
    inline auto* value()
    {
        return this;
    }

    //Read
    inline operator field_type() const requires(is_getter)
    { 
        return this->Getter();
    }

    inline auto* operator->() const requires(std::is_pointer_v<field_type> && is_getter)
    { 
        return this->Getter();
    }
    inline auto* operator->()const requires(is_ref_getter)
    { 
        return &this->Getter();
    }

    inline auto* operator&() requires(is_ref_getter)
    {
        return &this->Getter();
    }

    //Write
    inline TProperty& operator=(const TProperty& rhs) requires (is_setter)
    {
        if (this != &rhs)
        {
            this->Setter(rhs.Getter());
        }
        return *this;
    }
    inline TProperty& operator=(const remove_cvref_field_type& rhs) requires (is_setter)
    {
        this->Setter(rhs);
        return *this;
    }

    inline TProperty& operator+=(const TProperty& rhs) requires (is_setter)
    {
        if (this != &rhs)
        {
            this->Setter(this->Getter() + rhs.Getter());
        }
        return *this;
    }
    inline TProperty& operator+=(const remove_cvref_field_type& rhs) requires (is_setter)
    {
        this->Setter(this->Getter() + rhs);
        return *this;
    }

    inline TProperty& operator-=(const TProperty& rhs) requires (is_setter)
    {
        if (this != &rhs)
        {
            this->Setter(this->Getter() - rhs.Getter());
        }
        return *this;
    }
    inline TProperty& operator-=(const remove_cvref_field_type& rhs) requires (is_setter)
    {
        this->Setter(this->Getter() - rhs);
        return *this;
    }

    inline TProperty& operator*=(const TProperty& rhs) requires (is_setter)
    {
        if (this != &rhs)
        {
            this->Setter(this->Getter() * rhs.Getter());
        }
        return *this;
    }
    inline TProperty& operator*=(const remove_cvref_field_type& rhs) requires (is_setter)
    {
        this->Setter(this->Getter() * rhs);
        return *this;
    }

    inline TProperty& operator/=(const TProperty& rhs) requires (is_setter)
    {
        if (this != &rhs)
        {
            this->Setter(this->Getter() / rhs.Getter());
        }
        return *this;
    }
    inline TProperty& operator/=(const remove_cvref_field_type& rhs) requires (is_setter)
    {
        this->Setter(this->Getter() / rhs);
        return *this;
    }

};

//프로퍼티 클래스의 type을 가져오기 위한 헬퍼 템플릿
namespace PropertyUtils
{
    template <typename T>
    constexpr bool is_TProperty_v = false;

    template <typename owner_type, class getter, class setter>
    constexpr bool is_TProperty_v<TProperty<owner_type, getter, setter>> = true;

    template <typename T, typename = void>
    struct get_field_type
    {
        using type = T;
    };
    template <typename T>
    struct get_field_type<T, std::void_t<typename T::field_type>>
    {
        using type = typename T::field_type;
    };
    //field_type이 존재하면 해당 타입을 없으면 원본 타입 사용.
    template <typename T>
    using get_field_type_t = typename get_field_type<T>::type;

    template <typename T>
    concept is_setter = requires 
    {
        { T::is_setter } -> std::convertible_to<bool>;
    } && T::is_setter;
}