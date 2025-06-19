#pragma once

class Random
{
public:
    /// <summary>
    /// 시드를 고정하고 싶을때 사용합니다.
    /// </summary>
    /// <param name="seed :">사용할 시드</param>
    static void SetSeed(unsigned int seed);

    /// <summary>
    /// 무작위 int 난수를 생성합니다.
    /// </summary>
    /// <param name="min :">최소값 이상</param>
    /// <param name="max :">최대값 이하</param>
    /// <returns>결과</returns>
    static int Range(int min, int max)
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
    static float Range(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(engine);
    }

private:
    static std::mt19937 engine;
};