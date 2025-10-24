#pragma once

namespace QTE
{
    using Button = Input::Controller::Button;
    struct KeyBindState
    {
        Button ButtonX = Button::X;
        Button ButtonY = Button::Y;
        Button ButtonB = Button::B;
    };
    class KeyBinder
    {
    public:
        inline void ClearBindState() { _bindStateStack.clear(); }
        inline void PushKeyBindState(const KeyBindState& bindState) { _bindStateStack.push_back(bindState); }
        inline void PopKeyBindState() { _bindStateStack.pop_back(); }
        inline Button GetKeyX() const { return GetCurrentBindState().ButtonX; }
        inline Button GetKeyY() const { return GetCurrentBindState().ButtonY; }
        inline Button GetKeyB() const { return GetCurrentBindState().ButtonB; }
        inline const KeyBindState& GetCurrentBindState() const
        {
            if (_bindStateStack.empty())
            {
                return _defaultBindState;
            }
            return _bindStateStack.back();
        }

    public:
        KeyBindState _defaultBindState;
        std::vector<KeyBindState> _bindStateStack;
    };
}
