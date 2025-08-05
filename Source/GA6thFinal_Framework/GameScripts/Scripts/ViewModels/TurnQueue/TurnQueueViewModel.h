#pragma once

class TurnActor;

struct TurnUIData
{
    File::GuidRef ActorPortrait;
    File::GuidRef Frame;
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