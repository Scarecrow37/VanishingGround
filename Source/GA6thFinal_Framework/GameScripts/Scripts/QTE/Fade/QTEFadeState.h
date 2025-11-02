#pragma once

namespace QTE
{
    struct FadeState
    {
        // 0.0f ~ 1.0f 사이의 값. 
        float FadeInStartXFactor   = 0.0f;      // 기본 값일 시 판정선 x좌표에서 페이드 아웃 시작. 0.0f일 시 시작 지점.
        float FadeInEndXFactor     = 0.0f;      // 기본 값일 시 패널 끝 x좌표에서 페이드 아웃 종료. 1.0f일 시 종료 지점.
        float FadeOutStartXFactor  = 0.8f;     // 기본 값일 시 판정선 x좌표에서 페이드 아웃 시작. 0.0f일 시 시작 지점.
        float FadeOutEndXFactor    = 1.0f;      // 기본 값일 시 패널 끝 x좌표에서 페이드 아웃 종료. 1.0f일 시 종료 지점.

        // 이후 추가할거... 가이드 노트, QTE 종료 페이드 아웃 듀레이션 등...
    };
}
