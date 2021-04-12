//-----------------------------------------------------------------------------
// File: Inc_KdModelShader.hlsli
//
// Edit: 2021/04/02 �쓛����
//-----------------------------------------------------------------------------

// �萔�o�b�t�@(�I�u�W�F�N�g�P�ʍX�V)
cbuffer cbObject : register(b0)
{
    row_major float4x4  g_mW;       // ���[���h�ϊ��s��
    float2              g_uvOffset; //UV�I�t�Z�b�g
    float               tmp[2];
};

//------------------------------
// �萔�o�b�t�@(�}�e���A��)
//------------------------------

// �}�e���A���\����
struct Material
{
    float4  BaseColor;  // �x�[�X�F
    float3  Emissive;   // ���Ȕ����F
    float   Metallic;   // �����x
    float   Roughness;  // �e��
};

// �}�e���A��
cbuffer cbMaterial : register(b1)
{
    Material g_Material;
};

// ���_�V�F�[�_����o�͂���f�[�^
struct VSOutput
{
    float4 Pos      : SV_Position;  // �ˉe���W
    float2 UV       : TEXCOORD0;    // UV���W
    float4 Color    : TEXCOORD1;    // �F
    float3 wN       : TEXCOORD2;    // ���[���h�@��
    float3 wPos     : TEXCOORD3;    // ���[���h3D���W
    float3 wT       : TEXCOORD4;    // ���[���h�ڐ�(Tangent)
    float3 wB       : TEXCOORD5;    // ���[���h�]�@��(Binormal)
};