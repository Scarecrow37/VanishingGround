#pragma once
using namespace DirectX::SimpleMath;

// 씬그래프, position, rotation, scale을 제어하는 Transform 코어클래스
class Transform : public ReflectSerializer
{
    friend class ESceneManager;
    friend class EGameObjectFactory;
    USING_PROPERTY(Transform);

public:
    /*Transform의 좌표계 공간을 나타내는 enum class*/
    enum class Space
    {
        WORLD, // 월드
        LOCAL  // 로컬
    };

    Transform(GameObject& owner);
    ~Transform();

    /// <summary>
    /// Transform 값을 복사합니다.
    /// </summary>
    /// <param name="rhs :">복사할 대상</param>
    /// <param name="copyParent :">부모 복사 여부</param>
    /// <returns></returns>
    Transform& CopyTransform(const Transform& rhs, bool copyParent = true)
    {
        if (this != &rhs)
        {
            _hasChanged = true;
            _position   = rhs._position;
            _rotation   = rhs._rotation;
            _eulerAngle = rhs._eulerAngle;
            _scale      = rhs._scale;

            if (copyParent && nullptr != rhs._parent)
            {
                SetParent(rhs._parent, false);
            }
        }
        return *this;
    }

    Transform& operator=(const Transform& rhs)
    { 
       return CopyTransform(rhs);
    }

    GETTER_ONLY(GameObject&, gameObject)
    { 
        return _gameObject;
    }
    //get : owner GameObject
    PROPERTY(gameObject)

    GETTER_ONLY(int, ChildCount) 
    { 
        return GetChildCount();
    }
    int GetChildCount();
    // https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform-childCount.html
    // get : 자식의 개수를 반환합니다.
    // return : int
    PROPERTY(ChildCount)

    GETTER_ONLY(Transform*, Root) { return _root; }
    // https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform-root.html
    // get : 최상위 부모를 반환합니다.
    // return : Transform*
    PROPERTY(Root)

    GETTER_ONLY(Transform*, Parent) { return _parent; }
    // https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform-parent.html
    // get : 부모를 반환합니다.
    // return : Transform*
    PROPERTY(Parent)

    GETTER_ONLY(bool, HasChanged) { return _hasChanged; }
    // get : Transform의 이번 프레임 변경 여부를 확인합니다. true면 이번
    // 프레임에 행렬 계산 대상이 됩니다.
    PROPERTY(HasChanged)

    GETTER_ONLY(const Matrix&, LocalToWorldMatrix) { return GetWorldMatrix(); }
    /*
    get : 로컬 정점을 World 행렬로 변환하는 행렬입니다.
    (Transform의 World Matrix 입니다).
    */
    PROPERTY(LocalToWorldMatrix)

    GETTER_ONLY(const Matrix&, WorldToLocalMatrix)
    {
        return GetInversWorldMatrix();
    }
    /*
    get : 월드 행렬을 로컬 행렬로 변환하는 행렬입니다.
    (Transform의 World Invers Matrix 입니다.)
    */
    PROPERTY(WorldToLocalMatrix)

    GETTER_ONLY(const Matrix&, LocalToLocalMatrix) { return GetLocalMatrix(); }
    /*
    get : 로컬 정점을 LocalMatrix 행렬로 변환하는 행렬입니다.
    (Transform의 Local Matrix 입니다.)
    */
    PROPERTY(LocalToLocalMatrix)

    SETTER(const Vector3&, Position)
    {
        if (_position == value)
            return;

        _hasChanged = true;
        _position   = value;
    }
    GETTER(const Vector3&, Position) { return _position; }
    PROPERTY(Position)

    SETTER(const Quaternion&, Rotation)
    {
        if (_rotation == value)
            return;

        _hasChanged = true;
        _rotation   = value;
        _eulerAngle = _rotation.ToEuler() * Mathf::Rad2Deg;
    }
    GETTER(const Quaternion&, Rotation) { return _rotation; }
    PROPERTY(Rotation)

    GETTER_ONLY(const Vector3&, Forward) { return _forward; }
    PROPERTY(Forward)

    GETTER_ONLY(const Vector3&, Up) { return _up; }
    PROPERTY(Up)

    GETTER_ONLY(const Vector3&, Right) { return _right; }
    PROPERTY(Right)

    SETTER(const Vector3&, EulerAngle)
    {
        if (_eulerAngle == value)
            return;

        _hasChanged = true;
        _eulerAngle = value;
        Quaternion newRotation = Quaternion::CreateFromYawPitchRoll(_eulerAngle * Mathf::Deg2Rad);
        _rotation = newRotation;
    }
    GETTER(const Vector3&, EulerAngle) { return _eulerAngle; }
    PROPERTY(EulerAngle)

    SETTER(const Vector3&, Scale)
    {
        if (_scale == value)
            return;

        _hasChanged = true;
        _scale = value;
    }
    GETTER(const Vector3&, Scale) { return _scale; }
    PROPERTY(Scale)

    REFLECT_PROPERTY(Position, EulerAngle, Scale)  
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

    /// <summary>
    /// Transform를 DFS로 root부터 모든 자식들을 순회하면서 함수를 호출해줍니다.
    /// </summary>
    /// <typeparam name="Func">실행할 함수</typeparam>
    /// <param name="root :">DFS 시작할 루트</param>
    /// <param name="func : 실행할 함수"></param>
    inline static void ForeachDFS(Transform& root, const std::function<void(Transform*)>& func);

    /// <summary>
    /// Transform를 DFS로 root부터 모든 자식들을 순회하면서 함수를 호출해줍니다.
    /// </summary>
    /// <typeparam name="Func">실행할 함수</typeparam>
    /// <param name="root :">DFS 시작할 루트</param>
    /// <param name="func : 실행할 함수"></param>
    inline static void ForeachDFS(Transform& root, const std::function<void(Transform*, int)>& func);

    /// <summary>
    /// Transform를 BFS로 root부터 모든 자식들을 순회하면서 함수를 호출해줍니다.
    /// </summary>
    /// <typeparam name="Func">실행할 함수</typeparam>
    /// <param name="root">BFS 시작할 루트</param>
    /// <param name="func">실행할 함수</param>
    inline static void ForeachBFS(Transform& root, const std::function<void(Transform*)>& func); 

    /// <summary>
    /// Transform를 BFS로 root부터 모든 자식들을 순회하면서 함수를 호출해줍니다.
    /// </summary>
    /// <typeparam name="Func">실행할 함수</typeparam>
    /// <param name="root">BFS 시작할 루트</param>
    /// <param name="func">실행할 함수</param>
    inline static void ForeachBFS(Transform& root, const std::function<void(Transform*, int)>& func); 

public:
    /// <summary>
    /// 게임 오브젝트의 WeakPtr을 반환합니다.
    /// </summary>
    /// <returns></returns>
    std::weak_ptr<GameObject> GetWeakPtr();

    /// <summary>
    /// <para>
    /// https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform.DetachChildren.html
    /// </para> <para> 자식오브젝트들을 전부 분리해 root로 만들어버립니다.
    /// </para>
    /// </summary>
    void DetachChildren();

    /// <summary>
    /// <para>
    /// https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform.SetParent.html
    /// </para> <para> Transform의 부모를 설정합니다. nullptr 전달시 이
    /// Transform을 root로 만듭니다.               </para>
    /// </summary>
    /// <param name="p :">부모로 설정할 대상</param>
    /// <param name="worldPositionStays :">월드 좌표 유지 여부입니다.</param>
    void SetParent(Transform* p, bool worldPositionStays = true);
    void SetParent(Transform& p, bool worldPositionStays = true);

    /// <summary>
    /// <para>
    /// https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform.GetChild.html
    /// </para> <para> 인덱스로 자식을 가져옵니다. </para>
    /// </summary>
    /// <param name="index :">자식의 인덱스</param>
    /// <returns>성공시 해당 자식의 포인터. 실패시 nullptr</returns>
    Transform* GetChild(int index) const;

    /// <summary>
    /// https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform.Find.html
    /// <para> 이름으로 child를 찾아 반환합니다. </para> <para> 이름이 있는
    /// 자식을 찾을 수 없으면 null이 반환됩니다.'/' 문자가 포함된 경우 경로
    /// 이름처럼 계층 구조에서 Transform에 액세스합니다.      </para> <para>
    /// 참고: GameObject 이름에 '/'가 있는 경우 Find가 제대로 작동하지 않습니다.
    /// </para> <para> 참고 : Find는 Transform 계층 구조에서 재귀적으로 내려가지
    /// 않습니다. </para> <para> 참고 : Find는 비활성화된 GameObject의
    /// Transform을 찾을 수 있습니다. </para> <para> 참고 : 동일한 이름이
    /// 존재하면 첫번째 오브젝트를 반환합니다. </para>
    /// </summary>
    /// <param name="name :">찾을 오브젝트 이름</param>
    /// <returns></returns>
    Transform* Find(std::string_view name) const;

    /// <summary>
    /// <para>
    /// https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform.Rotate.html
    /// </para> <para> Transform을 특정 축으로 회전시킵니다. </para>
    /// </summary>
    /// <param name="axis :">회전시킬 축</param>
    /// <param name="angle :">각도. 단위 : 디그리드</param>
    /// <param name="relativeTo :">기준 축</param>
    inline void Rotate(const Vector3& axis, float angle,
                       Space relativeTo = Space::LOCAL)
    {
        Quaternion delta =
            Quaternion::CreateFromAxisAngle(axis, angle * Mathf::Deg2Rad);

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

    /// <summary>
    /// 월드 행렬 입니다.
    /// </summary>
    /// <returns></returns>
    const Matrix& GetWorldMatrix() { return _worldMatrix; }

    /// <summary>
    /// 로컬 행렬 입니다. 부모가 없으면 World 행렬과 동일합니다.
    /// </summary>
    /// <returns></returns>
    const Matrix& GetLocalMatrix() { return _localMatrix; }

    /// <summary>
    /// 월드 역행렬 입니다.
    /// </summary>
    /// <returns></returns>
    const Matrix& GetInversWorldMatrix() { return _inversWorldMatrix; }

private:
    GameObject& _gameObject;

private:
    Transform*              _root;
    Transform*              _parent;
    std::vector<Transform*> _childsList;

private:
    /// <summary>
    /// 부모를 지웁니다.
    /// </summary>
    void EraseParent(bool callEvent);

    /// <summary>
    /// 대상의 모든 자식을 순회하면서 root를 변경합니다.
    /// </summary>
    /// <param name="target :">루트</param>
    void SetChildsRootParent(Transform* root);

    /// <summary>
    /// 모든 부모를 확인하면서 전달받은 Transform이 존재하는지 확인합니다.
    /// </summary>
    /// <param name="potentialAncestor :">부모로 설정할 대상</param>
    /// <returns>존재 여부</returns>
    bool IsDescendantOf(Transform* potentialAncestor) const;

protected:
    /*직렬화 전 호출되는 함수*/
    void SerializedReflectEvent() override;
    /*역직렬화 후 호출되는 함수*/
    void DeserializedReflectEvent() override;

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    std::array<float, 3> position{};
    std::array<float, 4> rotation{};
    std::array<float, 3> eulerAngle{};
    std::array<float, 3> scale{};
    REFLECT_FIELDS_END(Transform)

private:
    bool       _hasChanged;
    Vector3    _position;
    Quaternion _rotation;
    Vector3    _forward;
    Vector3    _right;
    Vector3    _up;
    Vector3    _eulerAngle;
    Vector3    _scale;

    Matrix _worldMatrix;
    Matrix _inversWorldMatrix;
    Matrix _localMatrix;

private:
    /// <summary>
    /// 월드 행렬을 계산합니다.
    /// </summary>
    void UpdateMatrix();

    /// <summary>
    /// 내부에서 사용되는 SetParent 함수
    /// </summary>
    void SetParentEx(Transform* p, bool worldPositionStays, bool callEvent);

    /// <summary>
    /// 내부에서 사용되는 DetachChild 함수
    /// </summary>
    /// <param name="callEvent"></param>
    void DetachChildrenEx(bool callEvent);

    /// <summary>
    /// UI 컴포넌트들의 Detach 이벤트 함수를 호출합니다.
    /// </summary>
    /// <param name="target"></param>
    static void CallUIDetachParent(Transform* target, Transform* prevParent);

    /// <summary>
    /// UI 컴포넌트들의 Attach 이벤트 함수를 호출합니다.
    /// </summary>
    /// <param name="target"></param>
    static void CallUIAttachChild(Transform* target, Transform* newChild);

    /// <summary>
    /// object의 vaild 여부 체크합니다.
    /// </summary>
    /// <param name="target"></param>
    /// <returns></returns>
    static bool CheckValidTransform(Transform* target);

    /// <summary>
    /// Transform를 DFS로 root부터 모든 자식들을 순회하면서 함수를 호출해줍니다.
    /// </summary>
    /// <typeparam name="Func :">실행할 함수</typeparam>
    /// <param name="root :">루트</param>
    /// <param name="checkValid :">Valid 오브젝트 필터 여부</param>
    /// <param name="func"></param>
    inline static void ForeachExDFS(Transform& root, bool checkValid, const std::function<void(Transform*)>& func);

    /// <summary>
    /// Transform를 BFS로 root부터 모든 자식들을 순회하면서 함수를 호출해줍니다.
    /// </summary>
    /// <typeparam name="Func :">실행할 함수</typeparam>
    /// <param name="root :">루트</param>
    /// <param name="checkValid :">Valid 오브젝트 필터 여부</param>
    /// <param name="func"></param>
    inline static void ForeachExBFS(Transform& root, bool checkValid, const std::function<void(Transform*)>& func);

    /// <summary>
    /// Transform를 DFS로 root부터 모든 자식들을 순회하면서 함수를 호출해줍니다.
    /// </summary>
    /// <typeparam name="Func :">실행할 함수</typeparam>
    /// <param name="root :">루트</param>
    /// <param name="checkValid :">Valid 오브젝트 필터 여부</param>
    /// <param name="func"></param>
    inline static void ForeachExDFS(Transform& root, bool checkValid, const std::function<void(Transform*, int)>& func);

    /// <summary>
    /// Transform를 BFS로 root부터 모든 자식들을 순회하면서 함수를 호출해줍니다.
    /// </summary>
    /// <typeparam name="Func :">실행할 함수</typeparam>
    /// <param name="root :">루트</param>
    /// <param name="checkValid :">Valid 오브젝트 필터 여부</param>
    /// <param name="func"></param>
    inline static void ForeachExBFS(Transform& root, bool checkValid, const std::function<void(Transform*, int)>& func);
};

inline void Transform::ForeachDFS(Transform& root, const std::function<void(Transform*)>& func)
{
    ForeachExDFS(root, true, func);
}

inline void Transform::ForeachDFS(Transform& root, const std::function<void(Transform*, int)>& func)
{
    ForeachExDFS(root, true, func);
}

inline void Transform::ForeachBFS(Transform& root, const std::function<void(Transform*)>& func)
{
    ForeachExBFS(root, true, func);
}

inline void Transform::ForeachBFS(Transform& root, const std::function<void(Transform*, int)>& func)
{
    ForeachExBFS(root, true, func);
}

inline void Transform::ForeachExDFS(Transform& root, bool checkValid, const std::function<void(Transform*)>& func)
{
    std::vector<Transform*> trStack; // 순회용 stack (Foreach에서 씀)
    trStack.push_back(&root);
    while (trStack.empty() == false)
    {
        Transform* currTr = trStack.back();
        trStack.pop_back();
        bool vaild = checkValid ? CheckValidTransform(currTr) : true;
        if (vaild)
        {
            func(currTr);
            for (auto iter = currTr->_childsList.rbegin(); iter != currTr->_childsList.rend(); ++iter)
            {
                auto& transform = *iter;
                trStack.push_back(transform);
            }
        }
    }
}

inline void Transform::ForeachExDFS(Transform& root, bool checkValid, const std::function<void(Transform*, int)>& func)
{
    std::vector<std::pair<Transform*, int>> trStack; // 순회용 stack (Foreach에서 씀)
    trStack.emplace_back(&root, 0);
    while (trStack.empty() == false)
    {
        auto [currTr, currentDepth] = trStack.back();
        trStack.pop_back();
        bool vaild = checkValid ? CheckValidTransform(currTr) : true;
        if (vaild)
        {
            func(currTr, currentDepth);
            for (auto iter = currTr->_childsList.rbegin(); iter != currTr->_childsList.rend(); ++iter)
            {
                auto& transform = *iter;
                trStack.emplace_back(transform, currentDepth + 1);
            }
        }
    }
}

inline void Transform::ForeachExBFS(Transform& root, bool checkValid, const std::function<void(Transform*)>& func)
{
    std::queue<Transform*> trQueue; // 순회용 queue (Foreach에서 씀)
    trQueue.push(&root);
    while (trQueue.empty() == false)
    {
        Transform* currTr = trQueue.front();
        trQueue.pop();
        bool vaild = checkValid ? CheckValidTransform(currTr) : true;
        if (vaild)
        {
            func(currTr);
            for (auto& _transform : currTr->_childsList)
            {
                trQueue.push(_transform);
            }
        }
    }
}

inline void Transform::ForeachExBFS(Transform& root, bool checkValid, const std::function<void(Transform*, int)>& func)
{
    std::queue<std::pair<Transform*, int>> trQueue; // 순회용 queue (Foreach에서 씀)
    trQueue.push({&root, 0});
    while (trQueue.empty() == false)
    {
        auto [currTr, currentDepth] = trQueue.front();
        trQueue.pop();
        bool vaild = checkValid ? CheckValidTransform(currTr) : true;
        if (vaild)
        {
            func(currTr, currentDepth);
            for (auto& _transform : currTr->_childsList)
            {
                trQueue.push({_transform, currentDepth + 1});
            }
        }
    }
}
