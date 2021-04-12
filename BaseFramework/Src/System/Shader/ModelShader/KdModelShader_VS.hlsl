//-----------------------------------------------------------------------------
// File: KdModelShader_VS.hlsl
//
// Edit: 授業
//-----------------------------------------------------------------------------
#include "../inc_KdCommon.hlsli"
#include "inc_KdModelShader.hlsli"

//--------------------------------------------------
// 頂点シェーダ
//--------------------------------------------------
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0, float3 normal : NORMAL, float4 color : COLOR, float3 tangent : TANGENT)
{
    VSOutput Out;

    // 座標変換
    Out.Pos     = mul(pos, g_mW);       // ローカル座標系 -> 変換.ワールド座標系へ
    Out.wPos    = Out.Pos.xyz;	        // ワールド座標を出力
    Out.Pos     = mul(Out.Pos, g_mV);   // ワールド座標系 -> 変換.ビュー座標系へ
    Out.Pos     = mul(Out.Pos, g_mP);   // ビュー座標系   -> 変換.射影座標系へ

    
    Out.Color = color;// 頂点色
    Out.wN = normalize(mul(normal, (float3x3)g_mW));// 法線
    Out.wT = normalize(mul(tangent, (float3x3)g_mW));// 接線

    // 従接線
    float3 binormal = cross(normal, tangent);
    Out.wB = normalize(mul(binormal, (float3x3)g_mW));

    // UV座標(スクロール対応)
    Out.UV = uv + g_uvOffset;

    // 出力
    return Out;
}