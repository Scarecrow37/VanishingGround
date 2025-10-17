#pragma once

class TurnActor;

struct TurnUIData
{
    File::Guid ActorPortrait;
    File::Guid Frame;

    enum class ActorType
    {
        PLAYER,
        ENEMY_LEFT,
        ENEMY_RIGHT,
        ENEMY_MIDDLE
    };
    ActorType Type;
};

class TurnQueueViewModel final : public MVVM::ViewModel<std::deque<std::pair<int, TurnActor*>>, std::vector<TurnUIData>>
{
public:
    explicit TurnQueueViewModel(MVVM::Model<std::deque<std::pair<int, TurnActor*>>>& model);

protected:
    std::vector<TurnUIData> Convert(const std::deque<std::pair<int, TurnActor*>>& value) override;

private:
    std::vector<TurnUIData> _turnQueueData;
};