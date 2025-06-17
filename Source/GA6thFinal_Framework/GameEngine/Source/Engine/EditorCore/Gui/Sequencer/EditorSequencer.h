#pragma once

class EditorSequencer
{
public:
    EditorSequencer();
    virtual ~EditorSequencer();

public:
    void Render(TimelineSystem* system);
};
