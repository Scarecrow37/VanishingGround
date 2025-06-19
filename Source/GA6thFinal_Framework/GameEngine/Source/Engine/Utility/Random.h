#pragma once

class Random
{
public:
    /// <summary>
    /// 무작위 난수 시드를 생성합니다.
    /// </summary>
    Random()
    {
        std::random_device rd;
        engine.seed(rd());
    }

    /// <summary>
    /// 시드를 고정하고 싶을때 사용합니다.
    /// </summary>
    /// <param name="seed"></param>
    explicit Random(unsigned int seed) { engine.seed(seed); }

    /// <summary>
    /// 무작위 int 난수를 생성합니다.
    /// </summary>
    /// <param name="min :">최소값 이상</param>
    /// <param name="max :">최대값 이하</param>
    /// <returns>결과</returns>
    int Range(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine);
    }

    /// <summary>
    /// 무작위 float 난수를 생성합니다.
    /// </summary>
    /// <param name="min :">최소값 이상</param>
    /// <param name="max :">최대값 미만</param>
    /// <returns></returns>
    float Range(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(engine);
    }

private:
    std::mt19937 engine;
};