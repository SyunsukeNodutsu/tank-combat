//-----------------------------------------------------------------------------
// File: KdGenerateShadowMapShader_VS.hlsl
//
// Edit: 授業
//-----------------------------------------------------------------------------
#include "../inc_KdCommon.hlsli"
#include "inc_KdGenerateShadowMapShader.hlsli"

// 頂点シェーダ
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0)
{
	VSOutput Out;

	// ライトカメラで射影変換
	Out.Pos = mul(pos, g_mW);
	Out.Pos = mul(Out.Pos, g_mLightVP);

	// 射影座標
	Out.wvpPos = Out.Pos;
	// UV座標
	Out.UV = uv;

	return Out;
}