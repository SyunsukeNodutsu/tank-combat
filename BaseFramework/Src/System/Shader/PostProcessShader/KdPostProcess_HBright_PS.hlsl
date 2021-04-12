//-----------------------------------------------------------------------------
// File: KdPostProcess_HBright_PS.hlsl
//
// Edit: Žö‹Æ
//-----------------------------------------------------------------------------
#include "inc_KdPostProcess.hlsli"

Texture2D       g_inputTex  : register(t0);
SamplerState    g_ss        : register(s0);

float4 main(VSOutput In) : SV_Target0
{
    float4 texColor = g_inputTex.Sample(g_ss, In.UV);

    texColor.rgb = max(0, texColor.rgb - 1.0f);

    return texColor;
}