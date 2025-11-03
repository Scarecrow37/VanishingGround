#include "pchScripts.h"
#include "TransformNameTracker.h"

UMREAL_COMPONENT(TransformNameTracker)

TransformNameTracker::TransformNameTracker() = default;
TransformNameTracker::~TransformNameTracker() = default;


void TransformNameTracker::Added() 
{
    if (UmCore->IsPlay())
    {
        if (auto target = GameObject::Find(Name).lock())
        {
            transform->SetWorldMatrix(target->transform->GetWorldMatrix());
        }
    }
}