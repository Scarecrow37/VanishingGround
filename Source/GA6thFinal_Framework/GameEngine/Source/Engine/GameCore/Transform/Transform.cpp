#include "pch.h"

Transform::Transform(GameObject& owner)
    :
    _gameObject(owner),
    _root(nullptr),
    _parent(nullptr),

    _hasChanged(true),
    _position(),
    _rotation(), _scale(1, 1, 1) 
{
    auto CopyContext = [this](std::string_view name) 
    { 
        if (ImGui::BeginPopupContextItem(name.data()))
        {
            if (ImGui::MenuItem("Copy", "C") || ImGui::IsKeyReleased(ImGuiKey_C))
            {
                std::wstring transform = U8ToWString(SerializedReflectFields());
                File::SetClipboardText(transform);
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::BeginMenu("Paste"))
            {
                enum class PasteType
                {
                    NONE,
                    TRANSFORM,
                    POSITION,
                    ROTATION,
                    SCALE
                };
                PasteType pasteType = PasteType::NONE;
                bool isPaste = false;
                if (ImGui::MenuItem("Transform", "Q") || ImGui::IsKeyReleased(ImGuiKey_Q))
                {
                    pasteType = PasteType::TRANSFORM;
                    isPaste   = true;
                }
                if (ImGui::MenuItem("Position", "W") || ImGui::IsKeyReleased(ImGuiKey_W))
                {
                    pasteType = PasteType::POSITION;
                    isPaste   = true;
                }
                if (ImGui::MenuItem("Rotation", "E") || ImGui::IsKeyReleased(ImGuiKey_E))
                {
                    pasteType = PasteType::ROTATION;
                    isPaste   = true;
                }
                if (ImGui::MenuItem("Scale", "R") || ImGui::IsKeyReleased(ImGuiKey_R))
                {
                    pasteType = PasteType::SCALE;
                    isPaste   = true;
                }
                if (isPaste)
                {
                    std::wstring clipboardData = File::GetClipboardText();
                    if (false == clipboardData.empty())
                    {
                        std::string data = WStringToU8(clipboardData);
                        GameObject  dummyObject;
                        Transform   tempTransform(dummyObject);
                        if (tempTransform.DeserializedReflectFields(data))
                        {
                            switch (pasteType)
                            {
                            case PasteType::TRANSFORM:
                                CopyTransform(tempTransform, false);
                                break;
                            case PasteType::POSITION:
                                Position = tempTransform.Position;
                                break;
                            case PasteType::ROTATION:
                                Rotation = tempTransform.Rotation;
                                break;
                            case PasteType::SCALE:
                                Scale = tempTransform.Scale;
                                break;
                            default:
                                break;
                            }
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Math"))
            {
                static Vector3 positionTemp;
                static Vector3 rotationTemp;
                static Vector3 scaleTemp;

                static Vector3 positionTempMultiply(1, 1, 1);
                static Vector3 rotationTempMultiply(1, 1, 1);
                static Vector3 scaleTempMultiply(1, 1, 1);

                auto IsNoZeroField = [](const Vector3& vector3) 
                { 
                    return std::abs(vector3.x) > Mathf::Epsilon && 
                           std::abs(vector3.y) > Mathf::Epsilon &&
                           std::abs(vector3.z) > Mathf::Epsilon;
                };

                auto Vector3Draw = [this](const char* label, Vector3& vector3) 
                {
                    ImGui::DragFloat3(label, (float*)&vector3, 0.01f);
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {                     
                        return true;
                    }
                    return false;
                };
                if (ImGui::BeginMenu("+"))
                {           
                    if (Vector3Draw("Position", positionTemp))
                    {
                        Position += positionTemp;
                        positionTemp = Vector3(0, 0, 0);
                    }
                    if (Vector3Draw("Rotation", rotationTemp))
                    {
                        EulerAngle += rotationTemp;
                        rotationTemp = Vector3(0, 0, 0);
                    }
                    if (Vector3Draw("Scale", scaleTemp))
                    {
                        Scale += scaleTemp;
                        scaleTemp = Vector3(0, 0, 0);
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("-"))
                {
                    if (Vector3Draw("Position", positionTemp))
                    {
                        Position -= positionTemp;
                        positionTemp = Vector3(0, 0, 0);
                    }
                    if (Vector3Draw("Rotation", rotationTemp))
                    {
                        EulerAngle -= rotationTemp;
                        rotationTemp = Vector3(0, 0, 0);
                    }
                    if (Vector3Draw("Scale", scaleTemp))
                    {
                        Scale -= scaleTemp;
                        scaleTemp = Vector3(0, 0, 0);
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("*"))
                {
                    if (Vector3Draw("Position", positionTempMultiply))
                    {
                        if (IsNoZeroField(positionTempMultiply))
                        {
                            Position *= positionTempMultiply;
                            positionTempMultiply = Vector3(1, 1, 1);
                        }
                    }
                    if (Vector3Draw("Rotation", rotationTempMultiply))
                    {
                        if (IsNoZeroField(rotationTempMultiply))
                        {
                            EulerAngle *= rotationTempMultiply;
                            rotationTempMultiply = Vector3(1, 1, 1);
                        }
                    }
                    if (Vector3Draw("Scale", scaleTempMultiply))
                    {
                        if (IsNoZeroField(scaleTempMultiply))
                        {
                            Scale *= scaleTempMultiply;
                            scaleTempMultiply = Vector3(1, 1, 1);
                        }
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("/"))
                {
                    if (Vector3Draw("Position", positionTempMultiply))
                    {
                        if (IsNoZeroField(positionTempMultiply))
                        {
                            Position /= positionTempMultiply;
                            positionTempMultiply = Vector3(1, 1, 1);
                        }
                    }
                    if (Vector3Draw("Rotation", rotationTempMultiply))
                    {
                        if (IsNoZeroField(rotationTempMultiply))
                        {
                            EulerAngle /= rotationTempMultiply;
                            rotationTempMultiply = Vector3(1, 1, 1);
                        }
                    }
                    if (Vector3Draw("Scale", scaleTempMultiply))
                    {
                        if (IsNoZeroField(scaleTempMultiply))
                        {
                            Scale /= scaleTempMultiply;
                            scaleTempMultiply = Vector3(1, 1, 1);
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }
    };

    Position.SetInputAutoEvent(std::bind(CopyContext, Position.name()));
    EulerAngle.SetInputAutoEvent(std::bind(CopyContext, EulerAngle.name()));
    Scale.SetInputAutoEvent(std::bind(CopyContext, Scale.name()));
}
Transform::~Transform()
{
    DetachChildrenEx(false);
    EraseParent(true);
}

int Transform::GetChildCount()
{
    if constexpr (Application::IsEditor())
    {
        int validCount = 0;
        for (Transform* child : _childsList)
        {
            if (child && child->_gameObject.IsValid())
            {
                validCount++;
            }
        }
        return validCount;
    }
    else
    {
        return (int)_childsList.size();
    }
}

std::weak_ptr<GameObject> Transform::GetWeakPtr()
{
    return _gameObject.GetWeakPtr();
}

void Transform::DetachChildren()
{
    DetachChildrenEx(true);
}

void Transform::SetParent(Transform* p, bool worldPositionStays)
{
    SetParentEx(p, worldPositionStays, true);
}

void Transform::SetParent(Transform& p, bool worldPositionStays)
{
    SetParent(&p, worldPositionStays);
}

Transform* Transform::GetChild(int index) const
{
    Transform* child = nullptr;
    if (0 <= index)
    {
        if constexpr (Application::IsEditor())
        {
            int validChildCounter = 0;
            for (Transform* curr : _childsList)
            {
                if (curr && curr->_gameObject.IsValid())
                {
                    if (validChildCounter == index)
                    {
                        child = curr;
                        break;
                    }
                    ++validChildCounter;
                }
            }
        }
        else
        {
            if (index < _childsList.size())
            {
                child = _childsList[index];
            }
        }
    }
    return child;
}

void Transform::SetWorldPosition(const Vector3& position) 
{
    if (nullptr == _parent)
    {
        Position = position; //부모 없으면 그냥 설정
    }
    else
    {
        const Matrix& parentInversMatrix = _parent->GetInversWorldMatrix();
        Vector3       newPosition = Vector3::Transform(position, parentInversMatrix); // 목표 기준 로컬 좌표 구한다.
        Position                  = newPosition;
    }   
}

void Transform::EraseParent(bool callEvent)
{
    Transform* prevParent = this->_parent;
    if (prevParent)
    {
        if (!_parent->_childsList.empty())
        {
            std::erase(_parent->_childsList, this); //부모의 자식 항목에 자신을 제거
        }
        _root = nullptr;
        _parent = nullptr;
        SetChildsRootParent(this);
    }

    if (callEvent)
    {
        CallUIDetachParent(this, prevParent);
    }
}

bool Transform::IsDescendantOf(Transform* potentialAncestor) const
{
    Transform* currentParent = _parent;
    while (currentParent)
    {
        if (currentParent == potentialAncestor)
            return true;

        currentParent = currentParent->_parent;
    }
    return false;
}

bool Transform::IsPrefabDescendantOf(Transform* target) const
{
    const File::Guid& targetGuid = target->_gameObject.PrefabGuid;
    const Transform*  current    = this;
    while (current)
    {
        if (targetGuid != STR_NULL)
        {
            const File::Guid& currentGuid = current->_gameObject.PrefabGuid;
            if (currentGuid == targetGuid)
            {
                return true;
            }
        }
        current = current->_parent;
    }
    return false;
}

void Transform::SerializedReflectEvent()
{
    std::memcpy(ReflectFields->position.data(), & _position.x, sizeof(ReflectFields->position));
    std::memcpy(ReflectFields->rotation.data(), &_rotation.x, sizeof(ReflectFields->rotation));
    std::memcpy(ReflectFields->eulerAngle.data(), &_eulerAngle.x, sizeof(ReflectFields->eulerAngle));
    std::memcpy(ReflectFields->scale.data(), &_scale.x, sizeof(ReflectFields->scale));
}

void Transform::DeserializedReflectEvent()
{
    _position = Vector3(ReflectFields->position.data());
    _rotation = Quaternion(ReflectFields->rotation.data());
    _eulerAngle = Vector3(ReflectFields->eulerAngle.data());
    _scale = Vector3(ReflectFields->scale.data());

    _hasChanged = true;
}

void Transform::SetChildsRootParent(Transform* root)
{
    for (auto& child : _childsList)
    {
        Transform::ForeachDFS(
            *child, 
            [root](Transform* pTransform) 
            { 
                pTransform->_root = root; 
            });
    }
}

Transform* Transform::Find(std::string_view name) const
{
    for (int i = 0; i < ChildCount; i++)
    {
        if (Transform* child = GetChild(i))
        {
            GameObject& obj = child->gameObject;
            if ((const std::string&)obj.Name == name)
            {
                return child;
            }     
        }
    }
    return nullptr;
}

Transform* Transform::FindWithTag(const std::string& tag) const
{
    for (int i = 0; i < ChildCount; i++)
    {
        if (Transform* child = GetChild(i))
        {
            GameObject& obj = child->gameObject;
            if (obj.CompareTag(tag))
            {
                return child;
            }
        }
    }
    return nullptr;
}

void Transform::UpdateMatrix()
{
    Transform* root = _root ? _root : this;
    ForeachDFS(*root, [](Transform* curr) 
    {
        curr->_localMatrix = Matrix::CreateScale(curr->_scale) *
                             Matrix::CreateFromQuaternion(curr->_rotation) *
                             Matrix::CreateTranslation(curr->_position);
        if (curr->_parent == nullptr)
        {
            curr->_worldMatrix = curr->_localMatrix;
        }
        else
        {
            curr->_worldMatrix = curr->_localMatrix * curr->_parent->_worldMatrix;
        }
        curr->_inversWorldMatrix = curr->_worldMatrix.Invert();

        curr->_forward = Vector3(curr->_worldMatrix._31, curr->_worldMatrix._32, curr->_worldMatrix._33);
        curr->_forward.Normalize();

        curr->_up = Vector3(curr->_worldMatrix._21, curr->_worldMatrix._22, curr->_worldMatrix._23);
        curr->_up.Normalize();

        curr->_right = Vector3(curr->_worldMatrix._11, curr->_worldMatrix._12, curr->_worldMatrix._13);
        curr->_right.Normalize();

        curr->_worldPosition = curr->_worldMatrix.Translation();
    });
}

void Transform::SetParentEx(Transform* p, bool worldPositionStays, bool callEvent) 
{
    auto ComputeLocalTransform = [this, p, worldPositionStays]() {
        // World PositionStays 조건
        if (worldPositionStays)
        {
            const Matrix& myWorldMatrix = this->GetWorldMatrix();
            Matrix        myLocalMatrix;
            if (p)
            {
                const Matrix& parentWorldMatrix   = p->GetWorldMatrix();
                Matrix        parentInverseMatrix = parentWorldMatrix.Invert();
                myLocalMatrix                     = myWorldMatrix * parentInverseMatrix;
            }
            else
            {
                myLocalMatrix = myWorldMatrix;
            }
            myLocalMatrix.Decompose(_scale, _rotation, _position);
        }
    };

    if (p == nullptr)
    {
        Transform* prevParent = this->Parent;
        ComputeLocalTransform();
        EraseParent(callEvent);
    }
    else // 부모 관계 변경
    {      
        if (p->gameObject->GetOwnerSceneName() == UmSceneManager.DONT_DESTROY_ON_LOAD_SCENE_NAME)
        {
            GameObject::DontDestroyOnLoad(gameObject);
        }
        else if (p->gameObject->GetOwnerSceneName() != gameObject->GetOwnerSceneName())
        {
            return;
        }

        // 부모 관계가 가능한지 검증
        if (p != this->_parent)
        {
            if (p == this || p->IsDescendantOf(this))
            {
                return;
            }

            #ifdef _UMEDITOR
            if (true == p->IsPrefabDescendantOf(this))
            {
                UmLogger.Log(LogLevel::LEVEL_WARNING, u8"자신의 프리팹을 자식으로 넣을 수 없습니다.");
                return;
            }
            #endif 
        }

        Transform* prevParent = this->_parent;
        ComputeLocalTransform();
        // 부모 적용
        EraseParent(false);
        {
            _parent = p;

            if (p->_root)
                _root = p->_root;
            else
                _root = _parent;

            p->_childsList.push_back(this);
            SetChildsRootParent(_root);
        }

        if (callEvent)
        {
            CallUIDetachParent(this, prevParent);
            CallUIAttachChild(p, this);
        }       
    }
    _hasChanged = true;
    UpdateMatrix();
    GameObject::Engine::UpdateActiveInHierarchy(&_gameObject);
}

void Transform::SetParentToIndexEx(Transform* p, int index, bool worldPositionStays, bool callEvent) 
{
    SetParentEx(p, worldPositionStays, callEvent);
    std::vector<Transform*> sortTransforms;
    sortTransforms.reserve(p->_childsList.size());
    for (int i = (int)p->_childsList.size() - 1; i > index; --i)
    {
        sortTransforms.push_back(p->_childsList[i]);
    }
    for (auto& child : sortTransforms)
    {
        child->SetParentEx(p, worldPositionStays, callEvent);
    }
}

void Transform::DetachChildrenEx(bool callEvent) 
{
    for (auto& child : _childsList)
    {
        Transform* prevParent = child->_parent;
        if (nullptr != prevParent)
        {
            child->_root   = nullptr;
            child->_parent = nullptr;
            child->SetChildsRootParent(child);

            if (callEvent)
            {
                CallUIDetachParent(child, prevParent);
            }
        }
    }

    if (_childsList.empty() == false)
    {
        std::erase_if(_childsList, [](Transform* child) { return child->_parent == nullptr; });
    }
}

void Transform::CallUIDetachParent(Transform* target, Transform* prevParent)
{
    if (target)
    {
        GameObject& gameObject = target->gameObject;
        if (gameObject.IsValid())
        {
            for (size_t i = 0; i < gameObject.GetComponentCount(); ++i)
            {
                Component* component = gameObject.GetComponentAtIndex<Component>(i);
                if (Component::TYPE::UI == component->GetType())
                {
                    UIBaseComponent* uiBaseComponent = static_cast<UIBaseComponent*>(component);
                    GameObject*  prevObject  = nullptr;
                    if (prevParent)
                    {
                        prevObject = &prevParent->gameObject;
                    }
                    uiBaseComponent->OnDetachParent(prevObject);
                }
            }
        }
    }
}

void Transform::CallUIAttachChild(Transform* target, Transform* newChild)
{
    if (target)
    {
        GameObject& gameObject = target->gameObject;
        if (gameObject.IsValid())
        {
            for (size_t i = 0; i < gameObject.GetComponentCount(); ++i)
            {
                Component* component = gameObject.GetComponentAtIndex<Component>(i);
                if (Component::TYPE::UI == component->GetType())
                {
                    UIBaseComponent* uiBaseComponent = static_cast<UIBaseComponent*>(component);
                    GameObject*  newChildObject = nullptr;
                    if (newChild)
                    {
                        newChildObject = &newChild->gameObject;
                    }
                    uiBaseComponent->OnAttachChild(newChildObject);
                }
            }
        }
    }

    if (newChild)
    {
        GameObject& gameObject = newChild->gameObject;
        if (gameObject.IsValid())
        {
            for (size_t i = 0; i < gameObject.GetComponentCount(); ++i)
            {
                Component* component = gameObject.GetComponentAtIndex<Component>(i);
                if (Component::TYPE::UI == component->GetType())
                {
                    UIBaseComponent* uiBaseComponent = static_cast<UIBaseComponent*>(component);
                    GameObject*      targetObject  = nullptr;
                    if (target)
                    {
                        targetObject = &target->gameObject;
                    }
                    uiBaseComponent->OnAttachParent(targetObject);
                }
            }
        }
    }
}

bool Transform::CheckValidTransform(Transform* target)
{
    if (target)
    {
        return target->_gameObject.IsValid();
    }
    return false;
}


std::vector<GameObject*> Transform::FindDFSwithTag(const std::string& tag)
{
    std::vector<GameObject*> findResult;
    Transform::ForeachDFS(*this, [&](Transform* curr) 
    { 
        GameObject& object = curr->gameObject;
        if (true == object.CompareTag(tag))
        {
            findResult.push_back(&object);
        }
    });
    return findResult;
}

std::vector<GameObject*> Transform::FindBFSwithTag(const std::string& tag)
{
    std::vector<GameObject*> findResult;
    Transform::ForeachBFS(*this, [&](Transform* curr) 
    {
        GameObject& object = curr->gameObject;
        if (true == object.CompareTag(tag))
        {
            findResult.push_back(&object);
        }
    });
    return findResult;
}