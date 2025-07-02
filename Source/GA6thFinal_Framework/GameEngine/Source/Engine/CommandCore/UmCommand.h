#pragma once

class UmCommand
{
public:
    UmCommand(std::string_view name) : _name(name) {}
    virtual ~UmCommand() = default;
   
public:
    inline auto& GetName() const { return _name; }

public:
    virtual void Execute() = 0;

    virtual void Undo() = 0;

private:
    std::string _name;
};
