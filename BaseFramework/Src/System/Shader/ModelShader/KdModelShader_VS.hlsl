//-----------------------------------------------------------------------------
// File: KdModelShader_VS.hlsl
//
// Edit: ����
//-----------------------------------------------------------------------------
#include "../inc_KdCommon.hlsli"
#include "inc_KdModelShader.hlsli"

//--------------------------------------------------
// ���_�V�F�[�_
//--------------------------------------------------
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0, float3 normal : NORMAL, float4 color : COLOR, float3 tangent : TANGENT)
{
    VSOutput Out;

    // ���W�ϊ�
    Out.Pos     = mul(pos, g_mW);       // ���[�J�����W�n -> �ϊ�.���[���h���W�n��
    Out.wPos    = Out.Pos.xyz;	        // ���[���h���W���o��
    Out.Pos     = mul(Out.Pos, g_mV);   // ���[���h���W�n -> �ϊ�.�r���[���W�n��
    Out.Pos     = mul(Out.Pos, g_mP);   // �r���[���W�n   -> �ϊ�.�ˉe���W�n��

    
    Out.Color = color;// ���_�F
    Out.wN = normalize(mul(normal, (float3x3)g_mW));// �@��
    Out.wT = normalize(mul(tangent, (float3x3)g_mW));// �ڐ�

    // �]�ڐ�
    float3 binormal = cross(normal, tangent);
    Out.wB = normalize(mul(binormal, (float3x3)g_mW));

    // UV���W(�X�N���[���Ή�)
    Out.UV = uv + g_uvOffset;

    // �o��
    return Out;
}