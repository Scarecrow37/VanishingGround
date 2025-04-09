#pragma once
using namespace DirectX::SimpleMath;

//씬그래프, position, rotation, scale을 제어하는 Transform 코어클래스
class Transform :
    public ReflectSerializer
{
    friend class ESceneManager;
public:
    /*Transform의 좌표계 공간을 나타내는 enum class*/
    enum class Space
    {
        WORLD,  //월드
        LOCAL   //로컬
    };

    Transform(GameObject& owner);
    ~Transform();
    GameObject& gameObject;
public:
    /// <summary>
    /// 오일러 각으로 쿼터니언을 만듭니다. 매개변수의 단위는 디그리드 입니다.
    /// </summary>
    /// <param name="degAngle :">각도</param>
    /// <returns>쿼터니언. 단위 : 라디안</returns>
    inline static Quaternion EulerToQuaternion(const Vector3& degAngle)
    {
        return Quaternion::CreateFromYawPitchRoll(degAngle * Mathf::Deg2Rad);
    }

public:
    /// <summary>
    /// <para> https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform.DetachChildren.html </para>
    /// <para> 자식오브젝트들을 전부 분리해 root로 만들어버립니다.                                             </para>
    /// </summary>
    void DetachChildren();

    /// <summary>
    /// <para> https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform.SetParent.html  </para>
    /// <para> Transform의 부모를 설정합니다. nullptr 전달시 이 Transform을 root로 만듭니다.               </para>
    /// </summary>
    /// <param name="p :">부모로 설정할 대상</param>
    void SetParent(Transform* p);
    void SetParent(Transform& p);

    /// <summary>
    /// <para> https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform.GetChild.html </para>
    /// <para> 인덱스로 자식을 가져옵니다.                                                             </para>
    /// </summary>
    /// <param name="index :">자식의 인덱스</param>
    /// <returns>성공시 해당 자식의 포인터. 실패시 nullptr</returns>
    Transform* GetChild(int index) const
    {
        Transform* child = nullptr;
        if (index < _childsList.size())
        {
            child = _childsList[index];
        }
        return child;
    }

    /// <summary>
    /// https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform.Find.html
    /// <para> 이름으로 child를 찾아 반환합니다.                                                                                               </para>
    /// <para> 이름이 있는 자식을 찾을 수 없으면 null이 반환됩니다.'/' 문자가 포함된 경우 경로 이름처럼 계층 구조에서 Transform에 액세스합니다.      </para>
    /// <para> 참고: GameObject 이름에 '/'가 있는 경우 Find가 제대로 작동하지 않습니다.                                                          </para>
    /// <para> 참고 : Find는 Transform 계층 구조에서 재귀적으로 내려가지 않습니다.                                                               </para>
    /// <para> 참고 : Find는 비활성화된 GameObject의 Transform을 찾을 수 있습니다.                                                              </para>
    /// <para> 참고 : 동일한 이름이 존재하면 첫번째 오브젝트를 반환합니다.                                                                        </para>
    /// </summary>
    /// <param name="name :">찾을 오브젝트 이름</param>
    /// <returns></returns>
    Transform* Find(std::string_view name) const;

private:
    Transform* _root;
    Transform* _parent;
    std::vector<Transform*> _childsList;

private:
    /// <summary>
    /// 부모를 지웁니다.
    /// </summary>
    void EraseParent();

    /// <summary>
    /// 대상의 모든 자식을 순회하면서 root를 변경합니다.
    /// </summary>
    /// <param name="target :">루트</param>
    void SetChildsRootParent(Transform* Root);

    /// <summary>
    /// 모든 부모를 확인하면서 전달받은 Transform이 존재하는지 확인합니다.
    /// </summary>
    /// <param name="potentialAncestor :">부모로 설정할 대상</param>
    /// <returns>존재 여부</returns>
    bool IsDescendantOf(Transform* potentialAncestor) const;

public:
    USING_PROPERTY(Transform);
    GETTER_ONLY(int, ChildCount)
    {
        return _childsList.size();
    }
    // https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform-childCount.html
    //get : 자식의 개수를 반환합니다. 
    //return : int
    PROPERTY(ChildCount);

    GETTER_ONLY(Transform*, Root)
    {
        return _root;
    }
    // https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform-root.html
    //get : 최상위 부모를 반환합니다.
    //return : Transform*
    PROPERTY(Root);

    GETTER_ONLY(Transform*, Parent)
    {
        return _parent;
    }
    // https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform-parent.html
    //get : 부모를 반환합니다.
    //return : Transform*
    PROPERTY(Parent);

public:
    SETTER(const Vector3&, Position)
    {
        _isDirty = true;
        _position = value;
    }
    GETTER(const Vector3&, Position)
    {      
        return _position;
    }
    PROPERTY(Position)

    SETTER(const Quaternion&, Rotation)
    {
        _isDirty = true;
        _rotation = value;
        _eulerAngle = _rotation.ToEuler() * Mathf::Rad2Deg;
    }
    GETTER(const Quaternion&, Rotation)
    {
        return _rotation;
    }
    PROPERTY(Rotation)

    SETTER(const Vector3&, EulerAngle)
    {
         _isDirty = true;
         _eulerAngle = value;
         Quaternion newRotation = Quaternion::CreateFromYawPitchRoll(_eulerAngle * Mathf::Deg2Rad);
         _rotation = newRotation;
    }
    GETTER(const Vector3&, EulerAngle)
    {
        return _eulerAngle;
    }
    PROPERTY(EulerAngle)

    SETTER(const Vector3&, Scale)
    {
        _isDirty = true;
        _scale = value;
    }
    GETTER(const Vector3&, Scale)
    {
        return _scale;
    }
    PROPERTY(Scale)
    
    REFLECT_PROPERTY(
        Position, 
        EulerAngle,
        Scale)
public:
    /// <summary>
    /// <para> https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform.Rotate.html </para>
    /// <para> Transform을 특정 축으로 회전시킵니다. </para>
    /// </summary>
    /// <param name="axis :">회전시킬 축</param>
    /// <param name="angle :">각도. 단위 : 디그리드</param>
    /// <param name="relativeTo :">기준 축</param>
    inline void Rotate(const Vector3& axis, float angle, Space relativeTo = Space::LOCAL)
    {
        Quaternion delta = Quaternion::CreateFromAxisAngle(axis, angle * Mathf::Deg2Rad);

        if (relativeTo == Space::LOCAL)
        {
            Rotation = delta * Rotation;
        }
        else if (relativeTo == Space::WORLD)
        {
            Rotation = Rotation * delta;
        }
        Quaternion normalize;
        Rotation->Normalize(normalize);
        Rotation = normalize;
    }

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    std::array<float, 3> position{};
    std::array<float, 4> rotation{};
    std::array<float, 3> eulerAngle{};
    std::array<float, 3> scale{};
    REFLECT_FIELDS_END(Transform)

    /*직렬화 전 호출되는 함수*/
    void SerializedReflectEvent() override;
    /*역직렬화 후 호출되는 함수*/
    void DeserializedReflectEvent() override;
private:
    bool _isDirty;
    Vector3 _position;
    Quaternion _rotation;
    Vector3 _eulerAngle;
    Vector3 _scale;

    Matrix _worldMatrix;
    Matrix _localMatrix;
};

