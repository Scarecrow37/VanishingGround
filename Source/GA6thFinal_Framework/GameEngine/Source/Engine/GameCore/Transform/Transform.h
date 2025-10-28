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
    //type : GameObject&
    //get : owner GameObject
    PROPERTY(gameObject)

    GETTER_ONLY(int, ChildCount) 
    { 
        return GetChildCount();
    }
    int GetChildCount();
    // https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform-childCount.html
    // type : int
    // get : 자식의 개수를 반환합니다.
    // return : int
    PROPERTY(ChildCount)

    GETTER_ONLY(Transform*, Root) { return _root; }
    // https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform-root.html
    // type : Transform*
    // get : 최상위 부모를 반환합니다.
    // return : Transform*
    PROPERTY(Root)

    GETTER_ONLY(Transform*, Parent) { return _parent; }
    // https://docs.unity3d.com/6000.0/Documentation/ScriptReference/Transform-parent.html
    // type : Transform*
    // get : 부모를 반환합니다.
    // return : Transform*
    PROPERTY(Parent)

    inline void SetChangeFlag() { _hasChanged = true; }
    GETTER_ONLY(bool, HasChanged) { return _hasChanged; }
    // type : bool
    // get : Transform의 이번 프레임 변경 여부입니다. true면 이번 프레임에 행렬 계산 대상이 됩니다.
    PROPERTY(HasChanged)

    GETTER_ONLY(const Matrix&, LocalToWorldMatrix) { return GetWorldMatrix(); }
    /*
    type : const Matrix&
    get : 로컬 정점을 World 행렬로 변환하는 행렬입니다.
    (Transform의 World Matrix 입니다).
    */
    PROPERTY(LocalToWorldMatrix)

    GETTER_ONLY(const Matrix&, WorldToLocalMatrix)
    {
        return GetInversWorldMatrix();
    }
    /*
    type : const Matrix&
    get : 월드 행렬을 로컬 행렬로 변환하는 행렬입니다.
    (Transform의 World Invers Matrix 입니다.)
    */
    PROPERTY(WorldToLocalMatrix)

    GETTER_ONLY(const Matrix&, LocalToLocalMatrix) { return GetLocalMatrix(); }
    /*
    type : const Matrix&
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
    //type : const Vector3&
    //get, set : 이 Transform의 로컬 위치입니다. 
    PROPERTY(Position)

    SETTER(const Vector3&, WorldPosition) 
    { 
        SetWorldPosition(value);
    }
    GETTER(const Vector3&, WorldPosition) 
    { 
        return GetWorldPosition();
    }
    // type : const Vector3&
    // get, set : 이 Transform의 월드 기준 위치입니다. 
    PROPERTY(WorldPosition)

    SETTER(const Vector3&, LocalPosition)
    { 
        Position = value; 
    }
    GETTER(const Vector3&, LocalPosition)
    { 
        return Position;
    }
    // type : const Vector3&
    // get, set : 이 Transform의 로컬 위치입니다. 
    PROPERTY(LocalPosition)

    SETTER(const Quaternion&, Rotation)
    {
        if (_rotation == value)
            return;

        _hasChanged = true;
        _rotation   = value;
        _eulerAngle = _rotation.ToEuler() * Mathf::Rad2Deg;
    }
    GETTER(const Quaternion&, Rotation) { return _rotation; }
    // type : const Quaternion&
    // get, set : 이 Transform의 회전
    PROPERTY(Rotation)

    GETTER_ONLY(const Vector3&, Forward) { return _forward; }
    // type : const Vector3&
    // get : 이 Transform의 앞 방향 백터
    PROPERTY(Forward)

    GETTER_ONLY(const Vector3&, Up) { return _up; }
    // type : const Vector3&
    // get : 이 Transform의 위 방향 백터
    PROPERTY(Up)

    GETTER_ONLY(const Vector3&, Right) { return _right; }
    // type : const Vector3&
    // get : 이 Transform의 오른쪽 방향 백터
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
    // type : const Vector3&
    // get, set : 오일러 각도
    PROPERTY(EulerAngle)

    SETTER(const Vector3&, Scale)
    {
        if (_scale == value)
            return;

        _hasChanged = true;
        _scale = value;
    }
    GETTER(const Vector3&, Scale) { return _scale; }
    // type : const Vector3&
    // get, set : Transform의 Scale
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
    /// 루트 트랜스폼에서 시작하여 후위 순회 방식으로 모든 트랜스폼에 대해 지정된 함수를 호출합니다.
    /// </summary>
    /// <param name="root">순회를 시작할 루트 Transform 객체입니다.</param>
    /// <param name="func">각 Transform에 대해 호출할 함수 객체입니다. Transform 포인터를 인자로 받습니다.</param>
    inline static void ForeachPostOrder(Transform& root, const std::function<void(Transform*)>& func);

    /// <summary>
    /// 루트 트랜스폼에서 시작하여 후위 순회 방식으로 모든 트랜스폼에 대해 지정된 함수를 호출합니다.
    /// </summary>
    /// <param name="root">순회를 시작할 루트 Transform 객체입니다.</param>
    /// <param name="func">각 트랜스폼과 해당 깊이에 대해 호출되는 함수 객체입니다. 함수는 Transform 포인터와 int(깊이)를 인자로 받습니다.</param>
    inline static void ForeachPostOrder(Transform& root, const std::function<void(Transform*, int)>& func);

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
    /// 해당 태그를 가지고있는 자식을 찾아서 반환합니다. 여러개가 존재할 경우 가장 앞쪽에 있는 자식을 반환합니다.
    /// </summary>
    /// <param name="tag :">찾을 태그</param>
    /// <returns>없으면 nullptr</returns>
    Transform* FindWithTag(const std::string& tag) const;

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

    /// <summary>
    /// Dirty flag 레퍼런스를 반환합니다.
    /// </summary>
    /// <returns></returns>
    const bool& HasChangedRef() const { return _hasChanged; }

    /// <summary>
    /// 월드 기준으로 위치를 설정합니다.
    /// </summary>
    /// <param name="position :">설정할 위치</param>
    void SetWorldPosition(const Vector3& position);

    /// <summary>
    /// 월드 위치를 반환합니다.
    /// </summary>
    /// <returns></returns>
    const Vector3& GetWorldPosition()
    { 
        return _worldPosition;
    }

    /// <summary>
    /// 자식을 DFS로 순회하면서 해당 테그가 존재하는 오브젝트들을 반환합니다.
    /// </summary>
    /// <param name="tag :">찾을 태그</param>
    /// <returns>찾은 자식들</returns>
    std::vector<GameObject*> FindDFSwithTag(const std::string& tag);

    /// <summary>
    /// 자식을 BFS로 순회하면서 해당 테그가 존재하는 오브젝트들을 반환합니다.
    /// </summary>
    /// <param name="tag :">찾을 태그</param>
    /// <returns>찾은 자식들</returns>
    std::vector<GameObject*> FindBFSwithTag(const std::string& tag);

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
    /// <param name="potentialAncestor :">확인할 대상</param>
    /// <returns>존재 여부</returns>
    bool IsDescendantOf(Transform* potentialAncestor) const;

    /// <summary>
    /// 모든 부모를 확인하면서 전달받은 Prefab Guid가 존재하는지 확인합니다.
    /// </summary>
    /// <param name="potentialAncestor">확인할 대상</param>
    /// <returns></returns>
    bool IsPrefabDescendantOf(Transform* target) const;

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
    Vector3    _worldPosition;
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
    /// 특정 자식 인덱스에 삽입하는 함수
    /// </summary>
    void SetParentToIndexEx(Transform* p, int index, bool worldPositionStays, bool callEvent);

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
    /// object의 valid 여부 체크합니다.
    /// </summary>
    /// <param name="target"></param>
    /// <returns></returns>
    static bool CheckValidTransform(Transform* target);

    /// <summary>
    /// 루트 Transform에서 시작하여 후위 순회 방식으로 모든 Transform에 대해 지정된 함수를 호출합니다.
    /// </summary>
    /// <param name="root">순회를 시작할 루트 Transform 객체입니다.</param>
    /// <param name="checkValid">Transform의 유효성을 검사할지 여부를 지정하는 불리언 값입니다.</param>
    /// <param name="func">각 Transform에 대해 호출할 함수 객체입니다. Transform 포인터를 인자로 받습니다.</param>
    inline static void ForeachExPostOrder(Transform& root, bool checkValid,
                                          const std::function<void(Transform*)>& func);

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
    /// 루트 Transform에서 시작하여 후위 순회 방식으로 모든 Transform을 탐색하고, 각 노드에 대해 지정된 함수를 호출합니다.
    /// </summary>
    /// <param name="root">순회를 시작할 Transform 객체입니다.</param>
    /// <param name="checkValid">노드의 유효성을 검사할지 여부를 지정하는 불리언 값입니다.</param>
    /// <param name="func">각 Transform 노드와 해당 깊이에 대해 호출되는 함수 객체입니다. 함수는 (Transform*, int) 형식의 인자를 받습니다.</param>
    inline static void ForeachExPostOrder(Transform& root, bool checkValid,
                                          const std::function<void(Transform*, int)>& func);

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

inline void Transform::ForeachPostOrder(Transform& root, const std::function<void(Transform*)>& func)
{
    ForeachExPostOrder(root, true, func);
}

inline void Transform::ForeachPostOrder(Transform& root, const std::function<void(Transform*, int)>& func)
{
    ForeachExPostOrder(root, true, func);
}

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

inline void Transform::ForeachExPostOrder(Transform& root, bool checkValid, const std::function<void(Transform*)>& func)
{
    std::vector<Transform*> trStack; // 순회용 stack (Foreach에서 씀)
    trStack.push_back(&root);
    while (trStack.empty() == false)
    {
        Transform* currTr = trStack.back();
        trStack.pop_back();
        if (currTr)
        {
            bool valid = checkValid ? CheckValidTransform(currTr) : true;
            if (valid)
            {
                func(currTr);
                for (auto iter = currTr->_childsList.begin(); iter != currTr->_childsList.end(); ++iter)
                {
                    auto& transform = *iter;
                    trStack.push_back(transform);
                }
            }
        }
    }
}

inline void Transform::ForeachExDFS(Transform& root, bool checkValid, const std::function<void(Transform*)>& func)
{
    std::vector<Transform*> trStack; // 순회용 stack (Foreach에서 씀)
    trStack.push_back(&root);
    while (trStack.empty() == false)
    {
        Transform* currTr = trStack.back();
        trStack.pop_back();
        if (currTr)
        {
            bool valid = checkValid ? CheckValidTransform(currTr) : true;
            if (valid)
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
}

inline void Transform::ForeachExPostOrder(Transform& root, bool checkValid, const std::function<void(Transform*, int)>& func)
{
    std::vector<std::pair<Transform*, int>> trStack; // 순회용 stack (Foreach에서 씀)
    trStack.emplace_back(&root, 0);
    while (trStack.empty() == false)
    {
        auto& [currTr, currentDepth] = trStack.back();
        trStack.pop_back();
        if (currTr)
        {
            bool valid = checkValid ? CheckValidTransform(currTr) : true;
            if (valid)
            {
                func(currTr, currentDepth);
                for (auto iter = currTr->_childsList.begin(); iter != currTr->_childsList.end(); ++iter)
                {
                    auto& transform = *iter;
                    trStack.emplace_back(transform, currentDepth + 1);
                }
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
        auto& [currTr, currentDepth] = trStack.back();
        trStack.pop_back();
        if (currTr)
        {
            bool valid = checkValid ? CheckValidTransform(currTr) : true;
            if (valid)
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
}

inline void Transform::ForeachExBFS(Transform& root, bool checkValid, const std::function<void(Transform*)>& func)
{
    std::queue<Transform*> trQueue; // 순회용 queue (Foreach에서 씀)
    trQueue.push(&root);
    while (trQueue.empty() == false)
    {
        Transform* currTr = trQueue.front();
        trQueue.pop();
        if (currTr)
        {
            bool valid = checkValid ? CheckValidTransform(currTr) : true;
            if (valid)
            {
                func(currTr);
                for (auto& _transform : currTr->_childsList)
                {
                    trQueue.push(_transform);
                }
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
        auto& [currTr, currentDepth] = trQueue.front();
        trQueue.pop();
        if (currTr)
        {
            bool valid = checkValid ? CheckValidTransform(currTr) : true;
            if (valid)
            {
                func(currTr, currentDepth);
                for (auto& _transform : currTr->_childsList)
                {
                    trQueue.push({_transform, currentDepth + 1});
                }
            }
        }      
    }
}
