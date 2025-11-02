#include "pchScripts.h"
#include "Quit.h"

UMREAL_COMPONENT(Quit)

Quit::Quit() = default;
Quit::~Quit() = default;

void Quit::Submit()
{
    Base::Submit();
    UmApplication.Quit();
}