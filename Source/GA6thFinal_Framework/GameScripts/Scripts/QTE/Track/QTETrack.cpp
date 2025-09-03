#include "pchScripts.h"
#include "QTETrack.h"

namespace QTE
{
    Track::Track() 
        : _eventTrack(std::make_shared<Timeline::EventTrack>())
    {
    }

    Track::~Track() 
    {
    }
}