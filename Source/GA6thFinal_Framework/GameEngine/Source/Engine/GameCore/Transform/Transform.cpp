#include "pch.h"

Transform::Transform(GameObject& owner)
    :
    gameObject(owner),
    _root(nullptr),
    _parent(nullptr),

    _isDirty(true),
    _position(),
    _rotation(),
    _scale(1,1,1)
{
   

}
Transform::~Transform()
{
    EraseParent();
    std::vector<Transform*> transformStack;
    for (auto& transform : _childsList)
    {
        transformStack.push_back(transform);
    }
    this->DetachChildren();
    while (!_childsList.empty())
    {
        Transform* currTr = transformStack.back();
        transformStack.pop_back();
        for (auto& transform : currTr->_childsList)
        {
            transformStack.push_back(transform);
        }
        currTr->DetachChildren();
    }
}

void Transform::DetachChildren()
{
    for (auto& child : _childsList)
    {
        child->SetParent(nullptr);
    }
}

void Transform::SetParent(Transform* p)
{
    if (p == nullptr)
    {
        EraseParent();
    }
    else //부모 관계 변경
    {
        if (p == this || p == _parent || IsDescendantOf(this))
        {
            return;
        }
        EraseParent();
        {
            _parent = p;

            if (p->_root)
                _root = p->_root;
            else
                _root = _parent;

            p->_childsList.push_back(this);
            SetChildsRootParent(_root);
        }
    }
    _isDirty = true;
}

void Transform::SetParent(Transform& p)
{
    SetParent(&p);
}


void Transform::EraseParent()
{
    bool isParent = this->_parent != nullptr;
    if (isParent)
    {
        if (!_parent->_childsList.empty())
        {
            std::erase(_parent->_childsList, this); //부모의 자식 항목에 자신을 제거
        }
        _root = nullptr;
        _parent = nullptr;
        SetChildsRootParent(this);
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

    _isDirty = true;
}

void Transform::SetChildsRootParent(Transform* _root)
{
    std::vector<Transform*> transformStack;
    for (auto& tr : _childsList)
    {
        transformStack.push_back(tr);
    }
    while (!transformStack.empty())
    {
        Transform* curr = transformStack.back();
        transformStack.pop_back();

        curr->_root = _root;
        for (auto& tr : curr->_childsList)
        {
            transformStack.push_back(tr);
        }
    }
}

Transform* Transform::Find(std::string_view name) const
{
    for (int i = 0; i < ChildCount; i++)
    {
        if (Transform* child = GetChild(i))
        {
            GameObject& obj = child->gameObject;
            if (obj.Name == name)
            {
                return child;
            }     
        }
    }
    return nullptr;
}

