#pragma once

class UmCommand
{
public:
    UmCommand(std::string_view name) : _name(name) {}
    virtual ~UmCommand() = default;
   
public:
    inline auto& GetName() const { return _name; }

public:
    /// <summary>
    /// <para>커맨드 실행 시 작동할 로직을 구현합니다.</para>
    /// <para>true를 반환할 시 커맨드가 성공했다고 판단하여 Undo스택에 넣습니다.</para>
    /// <para>false를 반환할 시 커맨드가 실패했다고 판단하여 Undo스택에 넣지 않습니다.</para>
    /// </summary>
    /// <returns>로직 실행의 성공 여부</returns>
    virtual bool Execute() = 0;

    /// <summary>
    /// 커맨드를 되돌아갈 시 실행되는 로직을 구현합니다.
    /// </summary>
    virtual void Undo() = 0;

private:
    std::string _name;
};
