//-----------------------------------------------------------------------------
// File: KdPostProcess_Color_PS.hlsl
//
// Edit: 授業
//-----------------------------------------------------------------------------
#include "inc_KdPostProcess.hlsli"

Texture2D       g_inputTex  : register(t0);
SamplerState    g_ss        : register(s0);

cbuffer cb : register(b0)
{
    float4 g_color;
};

// ピクセルシェーダ
float4 main(VSOutput In) : SV_Target0
{
    float4 texColor = g_inputTex.Sample(g_ss, In.UV);
    return texColor * g_color;
}