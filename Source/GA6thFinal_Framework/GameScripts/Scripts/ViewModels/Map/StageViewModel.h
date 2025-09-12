#pragma once

class StageViewModel final : public MVVM::ViewModel<bool, File::Guid>
{
public:
    explicit StageViewModel(MVVM::Model<bool>& model, const File::Guid& enableGUID, const File::Guid& disableGUID);

protected:
    File::Guid Convert(const bool& value) override;

private:
    const File::Guid& _enableImage;
    const File::Guid& _disableImage;
};