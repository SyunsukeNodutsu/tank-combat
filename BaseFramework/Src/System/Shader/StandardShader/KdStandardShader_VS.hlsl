//-----------------------------------------------------------------------------
// File: KdStandardShader_VS.hlsl
//
// Edit: 初期配布
//-----------------------------------------------------------------------------
#include "../inc_KdCommon.hlsli"
#include "inc_KdStandardShader.hlsli"

//--------------------------------------------------
// Desc: 頂点シェーダ
// Arg : pos.頂点座標 uv.テクスチャUV座標 normal.法線ベクトル
//--------------------------------------------------
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0, float3 normal : NORMAL, float4 color : COLOR)
{
    VSOutput Out;

    // 座標系変換
    Out.Pos = mul(pos, g_mW);       // ローカル -> ワールド
    Out.wPos = Out.Pos.xyz;
    Out.Pos = mul(Out.Pos, g_mV);   // ワールド -> ビュー
    Out.Pos = mul(Out.Pos, g_mP);   // ビュー   -> 射影

    // 法線
    Out.wN = normalize(mul(normal, (float3x3)g_mW));

    // 頂点色
    Out.Color = color;

    // UV座標
    Out.UV = uv * g_UVTiling + g_UVOffset;

    // 出力
    return Out;
}