//-----------------------------------------------------------------------------
// File: KdPostProcess_VS.hlsl
//
// Edit: ����
//-----------------------------------------------------------------------------
#include "inc_KdPostProcess.hlsli"

// ���_�V�F�[�_
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0)
{
    VSOutput Out;
    Out.Pos = pos;
    Out.UV = uv;

    return Out;
}