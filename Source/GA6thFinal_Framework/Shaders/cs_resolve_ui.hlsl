#include "CommonData.hlsli"

struct LocalNode
{
    float4 Color;
    float Depth;
};

Texture2D<float4> screenTexture;
RWTexture2D<uint> OITHead;
RWStructuredBuffer<OITNode> OITNodes;
RWTexture2D<float4> Output;

[numthreads(16, 16, 1)]
void cs_main(uint3 tid : SV_DispatchThreadID)
{    
    uint2 pix = tid.xy;

    uint head = OITHead[pix];
    if (head == OIT_NULL)
    {
        Output[pix] = screenTexture.Load(int3(pix, 0));        
        return;
    }

    // 리스트 수집
    LocalNode nodes[OIT_MAX_LOCAL];
    uint count = 0;
    uint idx = head;
    while (idx != OIT_NULL && count < OIT_MAX_LOCAL)
    {
        OITNode node = OITNodes[idx];
        nodes[count].Color = node.Color;
        nodes[count].Depth = node.Depth;
        idx = node.Next;
        count++;
    }

    // (오버플로) count == OIT_MAX_LOCAL && idx != OIT_NULL -> 잘림 (디버그 필요)

    // Depth 오름차순(멀리 → 가까이) 삽입 정렬
    for (uint i = 1; i < count; ++i)
    {
        LocalNode key = nodes[i];
        int j = i - 1;
        while (j >= 0 && nodes[j].Depth < key.Depth)
        {
            nodes[j + 1] = nodes[j];
            j--;
        }
        nodes[j + 1] = key;
    }

    // 블렌딩(back-to-front: far→near)
    float4 outC = screenTexture.Load(int3(pix, 0));

    for (uint k = 0; k < count; ++k)
    {
        float4 src = nodes[k].Color; // PMA
        // src-over PMA
        outC.rgb = src.rgb + (1 - src.a) * outC.rgb;
        outC.a = src.a + (1 - src.a) * outC.a;
    }

    outC.a = 1;
    Output[pix] = outC;
}