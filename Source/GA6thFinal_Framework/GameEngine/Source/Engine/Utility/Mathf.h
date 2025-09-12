#pragma once


namespace Mathf
{
    enum EaseType
    {
        EASE_IN,
        EASE_OUT,
        EASE_IN_OUT,
        EASE_OUT_IN,
        EASE_COUNT
    };

    enum EaseFuncType
    {
        SINE,
        CUBIC,
        QUAD,
        QUART,
        QUINT,
        EXPO,
        CIRCULAR,
        BACK,
        ELASTIC,
        BOUNCE,
        COUNT
    };

    constexpr float FLOAT_MAX = FLT_MAX;
    constexpr float FLOAT_MIN = FLT_MIN;
    constexpr float PI        = 3.14159265f;
    constexpr float Deg2Rad   = 0.01745329f;
    constexpr float Rad2Deg   = 57.29578f;
    constexpr float Epsilon   = std::numeric_limits<float>::epsilon();
    /*radian angle 기준 Epsilon*/
    constexpr float AngleEpsilon = 0.001f;

    /*이거 왜 SimpleMath에 없음?*/
    constexpr Matrix IdentityMatrix = Matrix(1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1.f);
    // 선형 보간
    float Lerp(float startfloat, float endfloat, float t);



    // 두개의 쿼터니언 각도 차이를 반환해줍니다. 단위 : radian
    float GetAngleDifference(const DirectX::SimpleMath::Quaternion& q1, const DirectX::SimpleMath::Quaternion& q2);

    /* 전달받은 배열중 가장 작은 값을 찾습니다.*/
    template <typename T, std::size_t N>
    T FindMinValue(const T (&arr)[N])
    {
        return *std::min_element(arr, arr + N);
    };

    /* 전달받은 배열중 가장 높은 값을 찾습니다.*/
    template <typename T, std::size_t N>
    T FindMaxValue(const T (&arr)[N])
    {
        return *std::max_element(arr, arr + N);
    };

    template <typename T>
    T FindMinValue(T value)
    {
        return value;
    };
    /** 전달받은 인자들중 가장 낮은 값을 찾습니다.*/
    template <typename T, typename... Args>
    T FindMinValue(T first, Args... args)
    {
        return (std::min)(first, FindMinValue(args...));
    };

    template <typename T>
    T FindMaxValue(T value)
    {
        return value;
    };
    /** 전달받은 인자들중 가장 높은 값을 찾습니다.*/
    template <typename T, typename... Args>
    T FindMaxValue(T first, Args... args)
    {
        return (std::max)(first, FindMaxValue(args...));
    };

    /// <summary>
    /// 0이 되지 않도록 Clamp 합니다.
    /// </summary>
    /// <param name="value :">value</param>
    /// <param name="clampValue :">clamp value</param>
    /// <returns></returns>
    inline float ClampScale(float value, float clampValue)
    {
        if (value > 0.0f && value < clampValue)
            return clampValue;
        else if (value < 0.0f && value > -clampValue)
            return -clampValue;
        else if (value == 0.0f)
            return clampValue;
        return value;
    }

    /// <summary>
    /// easing 관련 연산 함수
    /// </summary>
    using EaseFunc      = float (*)(float);
    using EaseDerivFunc = float (*)(float);

    inline float InSine(float t)
    {
        return 1.0f - cosf(t * PI / 2.0f);
    }
    inline float OutSine(float t)
    {
        return sinf(t * PI / 2.0f);
    }
    inline float InCubic(float t)
    {
        return t * t * t;
    }
    inline float OutCubic(float t)
    {
        float inv = 1.0f - t;
        return 1.0f - inv * inv * inv;
    }
    inline float InQuad(float t)
    {
        return t * t;
    }
    inline float OutQuad(float t)
    {
        return 1.0f - (1.0f - t) * (1.0f - t);
    }
    inline float InQuart(float t)
    {
        return t * t * t * t;
    }
    inline float OutQuart(float t)
    {
        float inv = 1.0f - t;
        return 1.0f - inv * inv * inv * inv;
    }
    inline float InQuint(float t)
    {
        return t * t * t * t * t;
    }
    inline float OutQuint(float t)
    {
        float inv = 1.0f - t;
        return 1.0f - inv * inv * inv * inv * inv;
    }
    inline float InExpo(float t)
    {
        return (t == 0.0f) ? 0.0f : powf(2.0f, 10.0f * (t - 1.0f));
    }
    inline float OutExpo(float t)
    {
        return (t == 1.0f) ? 1.0f : 1.0f - powf(2.0f, -10.0f * t);
    }
    inline float InCircular(float t)
    {
        return 1.0f - sqrtf(1.0f - t * t);
    }
    inline float OutCircular(float t)
    {
        return sqrtf(1.0f - (t - 1.0f) * (t - 1.0f));
    }
    inline float InBack(float t)
    {
        float s = 1.70158f;
        return t * t * ((s + 1.0f) * t - s);
    }
    inline float OutBack(float t)
    {
        float s   = 1.70158f;
        float inv = t - 1.0f;
        return inv * inv * ((s + 1.0f) * inv + s) + 1.0f;
    }
    inline float InElastic(float t)
    {
        if (t == 0.0f)
            return 0.0f;
        if (t == 1.0f)
            return 1.0f;
        return -powf(2.0f, 10.0f * (t - 1.0f)) * sinf((t - 1.075f) * (2.0f * PI) / 0.3f);
    }
    inline float OutElastic(float t)
    {
        if (t == 0.0f)
            return 0.0f;
        if (t == 1.0f)
            return 1.0f;
        return powf(2.0f, -10.0f * t) * sinf((t - 0.075f) * (2.0f * PI) / 0.3f) + 1.0f;
    }
    inline float OutBounce(float t)
    {
        if (t < 1.0f / 2.75f)
            return 7.5625f * t * t;
        else if (t < 2.0f / 2.75f)
        {
            t -= 1.5f / 2.75f;
            return 7.5625f * t * t + 0.75f;
        }
        else if (t < 2.5f / 2.75f)
        {
            t -= 2.25f / 2.75f;
            return 7.5625f * t * t + 0.9375f;
        }
        else
        {
            t -= 2.625f / 2.75f;
            return 7.5625f * t * t + 0.984375f;
        }
    }
    inline float InBounce(float t)
    {
        return 1.0f - OutBounce(1.0f - t);
    }
    
    inline const std::vector<std::string> EaseNameTable = {"In", "Out", "In-Out", "Out-In"};

    inline const std::vector<std::string> EaseFuncNameTable = {"Sine", "Cubic",    "Quad", "Quart",   "Quint",
                                                               "Expo", "Circular", "Back", "Elastic", "Bounce"};

    inline const std::array<EaseFunc, EaseFuncType::COUNT * 2> EaseTable = {
        InSine, OutSine, InCubic,    OutCubic,    InQuad, OutQuad, InQuart,   OutQuart,   InQuint,  OutQuint,
        InExpo, OutExpo, InCircular, OutCircular, InBack, OutBack, InElastic, OutElastic, InBounce, OutBounce};

    /// <summary>
    /// blended easing 함수 입니다.
    /// </summary>
    /// <param name="beginType">도입부 easing type입니다.</param>
    /// <param name="endType">도출부 easing type입니다.</param>
    /// <param name="threshold">도입부와 도출부의 경계값 입니다.</param>
    /// <param name="t">현재 step 값 입니다.</param>
    /// <returns>easing 함수로 보간된 값을 반환합니다.</returns>
    inline float Ease(EaseType inoutType = EaseType::EASE_IN, EaseFuncType funcType = EaseFuncType::SINE,
                      float threshold = 0.5f, float t = 0.f)
    {
        t         = std::clamp(t, 0.0f, 1.0f);
        threshold = std::clamp(threshold, 0.0f, 1.0f);

        // 같은 타입이면 기존 방식 사용
        if (inoutType == EASE_IN || inoutType == EASE_OUT)
            return EaseTable[funcType * 2 + inoutType](t);

        EaseFunc inFunc  = EaseTable[funcType * 2];
        EaseFunc outFunc = EaseTable[funcType * 2 + 1];
        if (inoutType == EASE_IN_OUT)
        {
            if (t < threshold)
            {
                float u = t / threshold;
                return inFunc(u) * threshold;
            }
            else
            {
                float u = (t - threshold) / (1.0f - threshold);
                return threshold + outFunc(u) * (1.0f - threshold);
            }
        }
        else
        {
            if (t < threshold)
            {
                float u = t / threshold;
                return outFunc(u) * threshold;
            }
            else
            {
                float u = (t - threshold) / (1.0f - threshold);
                return threshold + inFunc(u) * (1.0f - threshold);
            }
        }
    }

    // 단일 float Catmull-Rom 보간 함수 (4점)
    inline float CatmullRomFloat(float t, float p0, float p1, float p2, float p3)
    {
        float t2 = t * t;
        float t3 = t2 * t;
        float a0 = -0.5f * p0 + 1.5f * p1 - 1.5f * p2 + 0.5f * p3;
        float a1 = p0 - 2.5f * p1 + 2.0f * p2 - 0.5f * p3;
        float a2 = -0.5f * p0 + 0.5f * p2;
        float a3 = p1;
        return a0 * t3 + a1 * t2 + a2 * t + a3;
    }

    // Vector3 Catmull-Rom 보간은 SimpleMath 함수 사용
    inline Vector3 CatmullRomVector3(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2,
                                     const Vector3& p3)
    {
        return Vector3::CatmullRom(p0, p1, p2, p3, t);
    }

    // 보간 함수 템플릿, float 또는 Vector3 대응
    template <typename T>
    T CatmullRomSpline(const std::vector<std::pair<float, T>>& points, float t)
    {
        t = std::clamp(t, points.front().first, points.back().first);

        size_t n = points.size();

        // 2점이면 선형 보간
        if (n == 2)
        {
            float t0     = points[0].first;
            float t1     = points[1].first;
            float localT = (t - t0) / (t1 - t0);

            if constexpr (std::is_same_v<T, float>)
            {
                return points[0].second + localT * (points[1].second - points[0].second);
            }
            else
            {
                return points[0].second.Lerp(points[1].second, localT);
            }
        }
        // 3점이면 중복점 만들기
        else if (n == 3)
        {
            size_t segment = 0;
            if (t >= points[1].first)
                segment = 1;

            float t0     = points[segment].first;
            float t1     = points[segment + 1].first;
            float localT = (t - t0) / (t1 - t0);

            // 4점 만들기: p0, p1, p2, p3
            const auto& p0 = (segment == 0) ? points[0].second : points[0].second;
            const auto& p1 = points[segment].second;
            const auto& p2 = points[segment + 1].second;
            const auto& p3 = (segment == 0) ? points[2].second : points[2].second;

            if constexpr (std::is_same_v<T, float>)
            {
                return CatmullRomFloat(localT, p0, p1, p2, p3);
            }
            else
            {
                return CatmullRomVector3(localT, p0, p1, p2, p3);
            }
        }
        // 4개 이상 일반 구간 보간
        else
        {
            // 구간 찾기
            size_t segment = 0;
            for (size_t i = 0; i < n - 1; ++i)
            {
                if (t >= points[i].first && t < points[i + 1].first)
                {
                    segment = i;
                    break;
                }
            }
            if (t == points.back().first)
            {
                segment = n - 2;
            }
            size_t i0 = (segment == 0) ? 0 : segment - 1;
            size_t i1 = segment;
            size_t i2 = segment + 1;
            size_t i3 = (segment + 2 >= n) ? n - 1 : segment + 2;

            float t0     = points[i1].first;
            float t1     = points[i2].first;
            float localT = (t - t0) / (t1 - t0);

            const auto& p0 = points[i0].second;
            const auto& p1 = points[i1].second;
            const auto& p2 = points[i2].second;
            const auto& p3 = points[i3].second;

            if constexpr (std::is_same_v<T, float>)
            {
                return CatmullRomFloat(localT, p0, p1, p2, p3);
            }
            else
            {
                return CatmullRomVector3(localT, p0, p1, p2, p3);
            }
        }
    }
} // namespace Mathf