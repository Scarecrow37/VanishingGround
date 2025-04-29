#pragma once

class Shader;
class Material
{
public:
    Material();
    virtual ~Material();

public:
    virtual void Render() = 0;
};