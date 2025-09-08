#pragma once

class Random
{
public:
    /// <summary>
    /// mt19937 엔진을 반환합니다
    /// </summary>
    /// <returns></returns>
    inline static std::mt19937& GetEngine() { return engine; }

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

    /// <summary>
    /// 가중치 목록을 담은 컨테이너를 받아 무작위 인덱스를 반환합니다.
    /// (std::vector, std::array 등 .begin()과 .end()가 있는 모든 컨테이너 사용 가능)
    /// </summary>
    /// <param name="weights">가중치 목록 컨테이너</param>
    /// <returns>선택된 인덱스</returns>
    template <typename T, typename = std::enable_if_t<!std::is_integral<T>::value>>
    static int Index(const T& weights)
    {
        std::discrete_distribution<int> dist(weights.begin(), weights.end());
        return dist(engine);
    }

    /// <summary>
    /// 0부터 size-1 까지의 범위에서 균등 확률로 무작위 인덱스를 반환합니다.
    /// </summary>
    /// <param name="size">아이템의 개수 또는 범위의 크기</param>
    /// <returns>0부터 size-1 사이의 랜덤 인덱스</returns>
    static int Index(int size)
    {
        if (size <= 0)
        {
            return 0;
        }
        std::uniform_int_distribution<int> dist(0, size - 1);
        return dist(engine);
    }

    /// <summary>
    /// 0부터 size-1 까지의 범위에서 균등 확률로 무작위 인덱스를 반환합니다.
    /// </summary>
    /// <param name="size">아이템의 개수 또는 범위의 크기</param>
    /// <returns>0부터 size-1 사이의 랜덤 인덱스</returns>
    static size_t Index(size_t size)
    {
        if (size <= 0)
        {
            return 0;
        }
        std::uniform_int_distribution<size_t> dist(0, size - 1);
        return dist(engine);
    }

private:
    static std::mt19937 engine;
};