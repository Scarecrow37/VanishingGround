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
        LINEAR,
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


    inline float InLinear(float t)
    {
        return 0;
    }
    inline float OutLinear(float t) 
    {
        return 0;
    }
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

    inline const std::vector<std::string> EaseFuncNameTable = {"Linear","Sine", "Cubic",    "Quad", "Quart",   "Quint",
                                                               "Expo", "Circular", "Back", "Elastic", "Bounce"};

    inline const std::array<EaseFunc, EaseFuncType::COUNT * 2> EaseTable = {
        InLinear, OutLinear, InSine, OutSine, InCubic,    OutCubic,    InQuad, OutQuad, InQuart,   OutQuart,   InQuint,  OutQuint,
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
        if (EaseFuncType::LINEAR == funcType)
        {
            return t;
        }
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
    inline float CatmullRomSpline(const std::vector<std::pair<float, float>>& points, float t)
    {
        t = std::clamp(t, points.front().first, points.back().first);
        size_t n = points.size();

        // 2점이면 선형 보간
        if (n == 2)
        {
            float t0     = points[0].first;
            float t1     = points[1].first;
            float localT = (t - t0) / (t1 - t0);
            return points[0].second + localT * (points[1].second - points[0].second);
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
            const auto& p0 = points[0].second;
            const auto& p1 = points[segment].second;
            const auto& p2 = points[segment + 1].second;
            const auto& p3 = points[2].second;

            return CatmullRomFloat(localT, p0, p1, p2, p3);
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

            return CatmullRomFloat(localT, p0, p1, p2, p3);
        }
    }
    inline float CatmullRomSpline(const std::vector<float>& steps, const std::vector<float>& points, float t)
    {
        t        = std::clamp(t, steps.front(), steps.back());
        size_t n = points.size();

        // 2점이면 선형 보간
        if (n == 2)
        {
            float t0     = steps[0];
            float t1     = steps[1];
            float localT = (t - t0) / (t1 - t0);
            return points[0] + localT * (points[1] - points[0]);
        }
        // 3점이면 중복점 만들기
        else if (n == 3)
        {
            size_t segment = 0;
            if (t >= steps[1])
                segment = 1;

            float t0     = steps[segment];
            float t1     = steps[segment + 1];
            float localT = (t - t0) / (t1 - t0);

            // 4점 만들기: p0, p1, p2, p3
            const auto& p0 = points[0];
            const auto& p1 = points[segment];
            const auto& p2 = points[segment + 1];
            const auto& p3 = points[2];

            return CatmullRomFloat(localT, p0, p1, p2, p3);
        }
        // 4개 이상 일반 구간 보간
        else
        {
            // 구간 찾기
            size_t segment = 0;
            for (size_t i = 0; i < n - 1; ++i)
            {
                if (t >= steps[i] && t < steps[i + 1])
                {
                    segment = i;
                    break;
                }
            }
            if (t == steps.back())
            {
                segment = n - 2;
            }
            size_t i0 = (segment == 0) ? 0 : segment - 1;
            size_t i1 = segment;
            size_t i2 = segment + 1;
            size_t i3 = (segment + 2 >= n) ? n - 1 : segment + 2;

            float t0     = steps[i1];
            float t1     = steps[i2];
            float localT = (t - t0) / (t1 - t0);

            const auto& p0 = points[i0];
            const auto& p1 = points[i1];
            const auto& p2 = points[i2];
            const auto& p3 = points[i3];

            return CatmullRomFloat(localT, p0, p1, p2, p3);
        }
    }
    inline Vector3 CatmullRomSpline(const std::vector<std::pair<float, Vector3>>& points, float t)
    {
        t = std::clamp(t, points.front().first, points.back().first);

        size_t n = points.size();

        // 2점이면 선형 보간
        if (n == 2)
        {
            float t0     = points[0].first;
            float t1     = points[1].first;
            float localT = (t - t0) / (t1 - t0);
            return Vector3::Lerp(points[0].second, points[1].second, localT);
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
            const auto& p0 = points[0].second;
            const auto& p1 = points[segment].second;
            const auto& p2 = points[segment + 1].second;
            const auto& p3 = points[2].second;
            return Vector3::CatmullRom(p0, p1, p2, p3, localT);
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
            return Vector3::CatmullRom(p0, p1, p2, p3, localT);
        }
    }
    inline Vector3 CatmullRomSpline(const std::vector<float>& steps, const std::vector<Vector3>& points, float t)
    {
        t = std::clamp(t, steps.front(), steps.back());

        size_t n = points.size();

        // 2점이면 선형 보간
        if (n == 2)
        {
            float t0     = steps[0];
            float t1     = steps[1];
            float localT = (t - t0) / (t1 - t0);
            return Vector3::Lerp(points[0], points[1], localT);
        }
        // 3점이면 중복점 만들기
        else if (n == 3)
        {
            size_t segment = 0;
            if (t >= steps[1])
                segment = 1;

            float t0     = steps[segment];
            float t1     = steps[segment + 1];
            float localT = (t - t0) / (t1 - t0);

            // 4점 만들기: p0, p1, p2, p3
            const auto& p0 = points[0];
            const auto& p1 = points[segment];
            const auto& p2 = points[segment + 1];
            const auto& p3 = points[2];
            return Vector3::CatmullRom(p0, p1, p2, p3, localT);
        }
        // 4개 이상 일반 구간 보간
        else
        {
            // 구간 찾기
            size_t segment = 0;
            for (size_t i = 0; i < n - 1; ++i)
            {
                if (t >= steps[i] && t < steps[i + 1])
                {
                    segment = i;
                    break;
                }
            }
            if (t == steps.back())
            {
                segment = n - 2;
            }
            size_t i0 = (segment == 0) ? 0 : segment - 1;
            size_t i1 = segment;
            size_t i2 = segment + 1;
            size_t i3 = (segment + 2 >= n) ? n - 1 : segment + 2;

            float t0     = steps[i1];
            float t1     = steps[i2];
            float localT = (t - t0) / (t1 - t0);

            const auto& p0 = points[i0];
            const auto& p1 = points[i1];
            const auto& p2 = points[i2];
            const auto& p3 = points[i3];
            return Vector3::CatmullRom(p0, p1, p2, p3, localT);
        }
    }
    inline Quaternion AlignHemisphere(const Quaternion& q, const Quaternion& ref)
    {
        return (ref.Dot(q) < 0.0f) ? Quaternion(-q.x, -q.y, -q.z, -q.w) : q;
    }
    inline Quaternion SafeSlerp(Quaternion a, Quaternion b, float t)
    {
        a.Normalize();
        b.Normalize();

        float dot = a.Dot(b);
        if (dot < 0.0f) // 반구 정렬
        {
            b   = Quaternion(-b.x, -b.y, -b.z, -b.w);
            dot = -dot;
        }

        // acos 도메인 보호
        dot = std::max(-1.0f, std::min(1.0f, dot));

        // 거의 동일 각: nlerp
        constexpr float kNearlyOne = 0.9995f; // ≈ 1.6°
        if (dot > kNearlyOne)
        {
            Quaternion r{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t};
            r.Normalize();
            return r;
        }

        const float theta    = std::acos(dot);
        const float sinTheta = std::sin(theta);
        if (sinTheta < 1e-6f) // 극소각 보호
            return a;

        const float s0 = std::sin((1.0f - t) * theta) / sinTheta;
        const float s1 = std::sin(t * theta) / sinTheta;

        Quaternion r{a.x * s0 + b.x * s1, a.y * s0 + b.y * s1, a.z * s0 + b.z * s1, a.w * s0 + b.w * s1};
        r.Normalize();
        return r;
    }
    inline Quaternion CatmullRomUniform4D(const Quaternion& q0, const Quaternion& q1, const Quaternion& q2, const Quaternion& q3, float u)
    {
        const float u2 = u * u;
        const float u3 = u2 * u;

        // 표준 Catmull-Rom(텐션 0)
        const float c0 = -0.5f * u3 + 1.0f * u2 - 0.5f * u;
        const float c1 = 1.5f * u3 - 2.5f * u2 + 1.0f;
        const float c2 = -1.5f * u3 + 2.0f * u2 + 0.5f * u;
        const float c3 = 0.5f * u3 - 0.5f * u2;

        Quaternion r{};
        r.x = c0 * q0.x + c1 * q1.x + c2 * q2.x + c3 * q3.x;
        r.y = c0 * q0.y + c1 * q1.y + c2 * q2.y + c3 * q3.y;
        r.z = c0 * q0.z + c1 * q1.z + c2 * q2.z + c3 * q3.z;
        r.w = c0 * q0.w + c1 * q1.w + c2 * q2.w + c3 * q3.w;
        r.Normalize();
        return r;
    }
    inline Quaternion CatmullRomSpline(const std::vector<float>& steps, const std::vector<Quaternion>& points, float t)
    {
        const size_t n = points.size();
        if (n == 0)
            return Quaternion::Identity;
        if (n == 1)
            return points[0];

        // 길이 일치 전제(필요하면 assert)
        assert(steps.size() == points.size() && "steps와 points의 길이는 같아야 함이다");

        // 요청한 엣지 리턴 분기
        if (t <= steps.front())
            return points.front();
        if (t >= steps.back())
            return points.back();

        // 세그먼트 찾기: steps[i1] <= t < steps[i2]
        auto   it = std::upper_bound(steps.begin(), steps.end(), t);
        size_t i1 = size_t(it - steps.begin() - 1);
        size_t i2 = i1 + 1;
        if (i1 >= n - 1)
        {
            i1 = n - 2;
            i2 = n - 1;
        }

        // 양 끝 보정 인덱스
        size_t i0 = (i1 == 0) ? 0 : i1 - 1;
        size_t i3 = (i2 + 1 >= n) ? n - 1 : i2 + 1;

        // 기준 p1에 대해 반구 정렬
        const Quaternion& p1ref = points[i1];
        Quaternion        p0    = AlignHemisphere(points[i0], p1ref);
        Quaternion        p1    = p1ref;
        Quaternion        p2    = AlignHemisphere(points[i2], p1ref);
        Quaternion        p3    = AlignHemisphere(points[i3], p1ref);

        // 로컬 파라미터 u ∈ [0,1]
        float denom = steps[i2] - steps[i1];
        if (denom <= 1e-7f)
            return p1; // 축퇴 방지
        const float u = (t - steps[i1]) / denom;

        // 큰 회전 구간 보호용 Slerp 폴백(임계값은 상황에 맞게 조정)
        {
            float d                  = std::fabs(p1.Dot(p2));
            d                        = std::min(1.0f, std::max(0.0f, d));
            const float     angle    = std::acos(d); // [0, π]
            constexpr float kBigTurn = 1.7453293f;   // ≈ 100°
            if (angle > kBigTurn)
            {
                Quaternion r = SafeSlerp(p1, p2, u);
                r            = AlignHemisphere(r, p1);
                r.Normalize();
                return r;
            }
        }

        // R^4 Catmull-Rom 보간
        Quaternion r = CatmullRomUniform4D(p0, p1, p2, p3, u);
        r            = AlignHemisphere(r, p1);
        r.Normalize();
        return r;
    }
    
    inline float Hash11(float p)
    {
        float s = sinf(p * 127.1f) * 43758.5453f;
        return s - floorf(s); // frac
    }
    inline float Perlin1D(float x)
    {
        float i0 = floorf(x);
        float i1 = i0 + 1.0f;
        float f  = x - i0; // [0,1) within cell

        // gradient ±1
        float g0 = (Hash11(i0) < 0.5f) ? -1.0f : 1.0f;
        float g1 = (Hash11(i1) < 0.5f) ? -1.0f : 1.0f;

        // 거리에 대한 기여
        float d0 = g0 * (f);        // dot at left
        float d1 = g1 * (f - 1.0f); // dot at right

        // Perlin fade (S-curve)
        float u = f * f * (3.0f - 2.0f * f);

        // 선형 보간
        float v = d0 + (d1 - d0) * u; // 이론상 [-1,1] 범주
        return v;
    }
    inline float FBM1D(float x, int octaves = 3, float lacunarity = 2.0f, float gain = 0.5f)
    {
        float amp  = 1.0f;
        float freq = 1.0f;
        float sum  = 0.0f;
        float norm = 0.0f;

        for (int i = 0; i < octaves; ++i)
        {
            sum += amp * Perlin1D(x * freq);
            norm += amp;
            freq *= lacunarity;
            amp *= gain;
        }

        // [-1,1] 정도로 정규화
        return (norm > 0.0f) ? (sum / norm) : 0.0f;
    }

} // namespace Mathf