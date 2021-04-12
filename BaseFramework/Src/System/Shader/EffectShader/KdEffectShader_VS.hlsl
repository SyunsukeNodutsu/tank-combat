//-----------------------------------------------------------------------------
// File: KdEffectShader_VS.hlsl
//
// Edit: 2021/03/27 野筒隼輔
//-----------------------------------------------------------------------------
#include "../inc_KdCommon.hlsli"
#include "inc_KdEffectShader.hlsli"

//-----------------------------------------------------------------------------
// Name: main()
// Desc: メインエントリ
//-----------------------------------------------------------------------------
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0, float4 color : COLOR)
{
    VSOutput Out;

    // ゆらゆらテスト
//    if (g_time > 0) {
//        const float height = 0.4f;
//        float wave = height * sin(g_time + pos.x * 100.0f);// *100 ... なめらかにする
//        pos.x += wave;
//    }

    // 座標変換
    Out.Pos = mul(pos, g_mW);       // ローカル座標系 -> ワールド座標系へ変換
    Out.Pos = mul(Out.Pos, g_mV);   // ワールド座標系 -> ビュー座標系へ変換
    Out.Pos = mul(Out.Pos, g_mP);   // ビュー座標系   -> 射影座標系へ変換

    // UV座標
    Out.UV = uv * g_UVTiling + g_UVOffset;

    // 色
    Out.Color = color;

    // 出力
    return Out;
}