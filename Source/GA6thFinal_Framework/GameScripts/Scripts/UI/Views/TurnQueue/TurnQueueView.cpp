#include "pchScripts.h"
#include "TurnQueueView.h"

#include "ViewModels/TurnQueue/TurnQueueViewModel.h"
#include "UI/Elements/Image/ImageElement.h"

UMREAL_COMPONENT(TurnQueueView)

TurnQueueView::TurnQueueView()
{
   
}

TurnQueueView::~TurnQueueView()
{
    UmWatcher.Blind<TurnQueueViewModel>("Turn Queue", _watchHandle);
}

void TurnQueueView::ImGuiDrawPropertysEvent() 
{
    if (ImGui::TreeNode("Childs Test"))
    {
        if (ImGui::Button("Find Childs"))
        {
            InitializeFramesAndPortraits();
        }
        if (ImGui::TreeNode("Frames"))
        {
            for (size_t i = 0; i < _turnQueueFrames.size(); i++)
            {
                std::string message = std::to_string(i);
                if (_turnQueueFrames[i])
                {
                    message += " frame ok";
                }
                else
                {
                    message += " frame nullptr";
                }
                ImGui::Text(message.c_str());
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Portraits"))
        {
            for (size_t i = 0; i < _turnQueuePortraits.size(); i++)
            {
                std::string message = std::to_string(i);
                if (_turnQueuePortraits[i])
                {
                    message += " portraits ok";
                }
                else
                {
                    message += " portraits nullptr";
                }
                ImGui::Text(message.c_str());
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

void TurnQueueView::Awake()
{
    Component::Awake();
    InitializeFramesAndPortraits();
}

void TurnQueueView::Start()
{
    Component::Start();
    _watchHandle = UmWatcher.Watch<TurnQueueViewModel, std::vector<TurnUIData>>("Turn Queue", [this](const std::vector<TurnUIData>& value) 
    {
       const size_t dataSize = value.size();
       for (size_t i = 0; i < _turnQueueFrames.size(); ++i)
       {
           if (i < dataSize)
           {
               if (nullptr != _turnQueueFrames[i])
               {
                   _turnQueueFrames[i]->Enable = true;
                   _turnQueueFrames[i]->SetImage(value[i].Frame);
               }
               if (nullptr != _turnQueuePortraits[i])
               {
                   _turnQueuePortraits[i]->Enable = true;
                   _turnQueuePortraits[i]->SetImage(value[i].ActorPortrait);
               }
           }
           else
           {
               if (nullptr != _turnQueueFrames[i])
               {
                   _turnQueueFrames[i]->Enable = false;
               }
               if (nullptr != _turnQueuePortraits[i])
               {
                   _turnQueuePortraits[i]->Enable = false;
               }
           }

       }
    });
}

enum class FindResult
{
    FIND,
    NOT_EXIST_IMAGE_ELEMENT,
    NOT_EXIST_GAME_OBJECT,
};

ImageElement* TurnQueueView::FindImageElementWithTag(const std::string& tag) const
{
    FindResult    result   = FindResult::NOT_EXIST_GAME_OBJECT;
    ImageElement* element  = nullptr;
    std::vector<GameObject*> findResult = _turnQueueHorizontalPanenl->transform->FindBFSwithTag(tag);
    if (false == findResult.empty())
    {
        GameObject* object = findResult.front();
        if (element = object->GetComponent<ImageElement>())
        {
            result = FindResult::FIND;
        }
        else
        {
            result = FindResult::NOT_EXIST_IMAGE_ELEMENT;
        }
    }

    if (result == FindResult::NOT_EXIST_IMAGE_ELEMENT)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING,
                     tag + reinterpret_cast<const char*>(u8" GameObject에 ImageElement가 없습니다."));
    }
    else if (result == FindResult::NOT_EXIST_GAME_OBJECT)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, tag + reinterpret_cast<const char*>(u8" GameObject를 찾을 수 없습니다."));
    }

    return element;
}


void TurnQueueView::InitializeFramesAndPortraits()
{
    if (_turnQueueHorizontalPanenl = GameObject::FindWithTag("Turn Queue Horizontal Panenl").lock().get())
    {
        FindFramesWithTag("Frame Element");
        FindPortraitsWithTag("Turn Element");
        gameObject->ActiveSelf = false;
    }
    else
    {
        std::u8string message = u8"Turn Queue Horizontal Panenl이 존재하지 않습니다.";
        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
    }
}

void TurnQueueView::FindFramesWithTag(const std::string& tag) 
{
    std::vector<GameObject*> findResult = _turnQueueHorizontalPanenl->transform->FindBFSwithTag(tag);
    for (size_t i = 0; i < findResult.size(); ++i)
    {
        if (i < _turnQueueFrames.size())
        {
            GameObject* object = findResult[i];
            if (_turnQueueFrames[i] = object->GetComponent<ImageElement>(); nullptr == _turnQueueFrames[i])
            {
                UmLogger.Log(LogLevel::LEVEL_WARNING, tag + (const char*)u8" GameObject에 ImageElement가 없습니다.");
            }
        }
    }
}

void TurnQueueView::FindPortraitsWithTag(const std::string& tag) 
{

    std::vector<GameObject*> findResult = _turnQueueHorizontalPanenl->transform->FindBFSwithTag(tag);
    for (size_t i = 0; i < findResult.size(); ++i)
    {
        if (i < _turnQueuePortraits.size())
        {
            GameObject* object = findResult[i];
            if (_turnQueuePortraits[i] = object->GetComponent<ImageElement>(); nullptr == _turnQueuePortraits[i])
            {
                UmLogger.Log(LogLevel::LEVEL_WARNING, tag + (const char*)u8" GameObject에 ImageElement가 없습니다.");
            }
        }
    }
}
