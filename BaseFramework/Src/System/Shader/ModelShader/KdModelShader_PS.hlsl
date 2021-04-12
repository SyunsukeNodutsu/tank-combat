//-----------------------------------------------------------------------------
// File: KdModelShader_PS.hlsl
//
// Edit: ����
//-----------------------------------------------------------------------------
#include "../inc_KdCommon.hlsli"
#include "inc_KdModelShader.hlsli"

//--------------------------------------------------
// �e�N�X�`���i�摜�f�[�^�����̂܂܂ł͎g���Ȃ��j
//--------------------------------------------------
Texture2D g_baseTex			: register(t0);		// �x�[�X�J���[�e�N�X�`��
Texture2D g_emissiveTex		: register(t1);		// �G�~�b�V�u�e�N�X�`��
Texture2D g_mrTex			: register(t2);		// ���^���b�N/���t�l�X�e�N�X�`��
Texture2D g_normalTex		: register(t3);		// �@���}�b�v
Texture2D g_dirShadowMap	: register(t102);	// ���s���V���h�E�}�b�v(0�`127�g�p�\�ŁA�����102���g�p)

//--------------------------------------------------
// �T���v���i�e�N�X�`���̃f�[�^��������ځj
//--------------------------------------------------
SamplerState			g_ss				: register(s0);
SamplerComparisonState	g_ssShadowMapComp	: register(s10);

//--------------------------------------------------
// BlinnPhong NDF
// �ElightDir    �c ���C�g�̕���
// �EvCam        �c �s�N�Z������J�����ւ̕���
// �Enormal      �c �@��
// �EspecPower   �c ���˂̉s��
//--------------------------------------------------
float BlinnPhong(float3 lightDir, float3 vCam, float3 normal, float specPower)
{
	float3 H = normalize(-lightDir + vCam);
	float NdotH = saturate(dot(normal, H)); // �J�����̊p�x��(0�`1)
	float spec = pow(NdotH, specPower);

	// ���K��Blinn-Phong
	return spec * ((specPower + 2) / (2 * 3.1415926535));
}

//
float CheckShadow(VSOutput In)
{
	// �ŏI�I�Ȓl
	float shadow = 1.0f;

	// �s�N�Z����3D���W����A�V���h�E�}�b�v��Ԃ֕ϊ�
	float4 liPos = mul(float4(In.wPos, 1), g_DL_mLightVP);

	// ���g�Ŏˉe���W�ɕϊ�����ꍇ�͉��s�ŕϊ����Ă��(���������܂�)
	liPos.xyz /= liPos.w;
	float shadowX = abs(liPos.x);
	float shadowY = abs(liPos.y);

	// �[�x�}�b�v�͈͓̔����m�F
	if (shadowX <= 1 && shadowY <= 1 && liPos.z <= 1)
	{
		// �ˉe���W -> UV���W�֕ϊ� ���E�����g�傷��΂�������(�C���[�W)
		float2 uv = liPos.xy * float2(1, -1) * 0.5 + 0.5;

		// ���C�g�J��������̋��� ���V���h�E�A�N�l�΍�(ImGui�ŕύX�ł���悤�ɏC������)
		float z = liPos.z - 0.002f;

		// �ڂ���
		float width, height;
		g_dirShadowMap.GetDimensions(width, height);
		float tw = 1.0f / width;
		float th = 1.0f / height;

		// UV�̎���3x3���l�����A���ϒl���Z�o
		shadow = 0;
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				// �e����
				shadow += g_dirShadowMap.SampleCmpLevelZero(g_ssShadowMapComp, uv + float2(x * tw, y * th), z);
			}
		}
		shadow *= 0.11f;

		// UV���W���痣��Ă���Ɣ���
		shadow = min(1.0f, shadow + pow(shadowX, 3.0f) + pow(shadowY, 3.0f));
	}

	return shadow;
}

//--------------------------------------------------
// �s�N�Z���V�F�[�_
//--------------------------------------------------
float4 main(VSOutput In) : SV_Target0
{
	// �J�����ւ̕���
	float3 vCam = g_CamPos - In.wPos;
	float camDist = length(vCam); // �J���� - �s�N�Z������
	vCam = normalize(vCam);

	// �R��̖@������@���s����쐬
	row_major float3x3 mTBN = {
		normalize(In.wT),
		normalize(In.wB),
		normalize(In.wN),
	};

	// �@���}�b�v����@���x�N�g���擾
	float3 wN = g_normalTex.Sample(g_ss, In.UV).rgb;

	// 0�`1�@���� -1�`1�@�֕ϊ�
	wN = wN * 2.0f - 1.0f;

	// �@���x�N�g�������̃s�N�Z����Ԃ֕ϊ�
	wN = normalize(mul(wN, mTBN));

	//------------------------------------------
	// �ގ��F
	//------------------------------------------

	// �ގ��̐F
	float4 baseColor = g_baseTex.Sample(g_ss, In.UV) * g_Material.BaseColor * In.Color;

	//
	float4 mr = g_mrTex.Sample(g_ss, In.UV);

	// ������
	float metallic = mr.b * g_Material.Metallic;

	// �e��
	float roughness = mr.b * g_Material.Roughness;

	// ���t�l�X����A���ˌ��p��SpecularPower�����߂�
	// Call of Duty : Black Ops��FarCry3�ł̎�@���Q�l
	// specPower = 2^(13*g)�@�O���X�lg = 0�`1��1�`8192�̃X�y�L�������x�����߂�
	// �����ł� specPower = 2^(11*g)�@1�`2048���g�p���܂��B
	// �Q�l�Fhttps://hanecci.hatenadiary.org/entry/20130511/p1
	float smoothness = 1.0f - roughness; // ���t�l�X���t�]�����u���炩�v���ɂ���
	float specPower = pow(2.0f, 11.0f * smoothness); // 1�`2048

	//------------------------------------------
	// ���C�e�B���O
	//------------------------------------------

	// �ŏI�I�ȐF
	float3 color = 0.0f;

	// �ގ��̊g�U�F�@������قǍގ��̐F�ɂȂ�A�����قǊg�U�F�͖����Ȃ�
	const float3 baseDiffuse = lerp(baseColor.rgb, float3(0.0f, 0.0f, 0.0f), metallic);

	// �ގ��̔��ːF�@������قǌ��̐F�����̂܂ܔ��˂��A�����قǍގ��̐F�����
	const float3 baseSpecular = lerp(0.04f, baseColor.rgb, metallic);

	//--------------------------------------------------
	// �V���h�E�}�b�s���O(�e����)
	//--------------------------------------------------
	float shadow = CheckShadow(In);

	//------------------------------------------
	// ���s��
	//------------------------------------------

	// Diffuse(�g�U��)
	{
		// ���̕����Ɩ@���̕����Ƃ̊p�x�������̋����ɂȂ�
		float lightDiffuse = dot(-g_DL_Dir, wN);
		lightDiffuse = saturate(lightDiffuse);  // �}�C�i�X�l��0�ɂ���@0(��)�`1(��)�ɂȂ�

		// ���K��Lambert
		lightDiffuse /= 3.1415926535;

		// ���̐F * �ގ��̊g�U�F * ������
		color += (g_DL_Color * lightDiffuse) * baseDiffuse * baseColor.a * shadow;
	}

	// Specular(���ːF) �����˂������̋��������߂�
	{
		// Blinn-Phong NDF
		float spec = BlinnPhong(g_DL_Dir, vCam, wN, specPower);

		// ���̐F * ���ˌ��̋��� * �ގ��̔��ːF * ������ * �K���Ȓ����l
		color += (g_DL_Color * spec) * baseSpecular * baseColor.a * shadow;
	}

	//------------------------------------------
	// �_��
	//------------------------------------------
	for (int pi = 0; pi < g_PL_Cnt.x; pi++)
	{
		// �s�N�Z������_���ւ̕���
		float3 dir = g_PL[pi].Pos - In.wPos;

		// �������Z�o
		float dist = length(dir);

		// ���K��
		dir /= dist;

		// �_���̔���ȓ�
		if (dist < g_PL[pi].Radius)
		{
			// ���a�����ƂɁA�����̔䗦�����߂�
			float atte = 1.0 - saturate(dist / g_PL[pi].Radius);
			// �t�Q��̖@��
			atte *= atte;

			// Diffuse(�g�U��)
			{
				// ���̕����Ɩ@���̕����Ƃ̊p�x�������̋����ɂȂ�
				float lightDiffuse = dot(dir, wN);
				lightDiffuse = saturate(lightDiffuse); // �}�C�i�X�l��0�ɂ���@0(��)�`1(��)�ɂȂ�

				lightDiffuse *= atte; // ����

				// ���K��Lambert
				lightDiffuse /= 3.1415926535;

				// ���̐F * �ގ��̊g�U�F * ������
				color += (g_PL[pi].Color * lightDiffuse) * baseDiffuse * baseColor.a;
			}

			// Specular(���ːF)
			{
				// ���˂������̋��������߂�
				// Blinn-Phong NDF
				float spec = BlinnPhong(-dir, vCam, wN, specPower);

				spec *= atte; // ����

				// ���̐F * ���ˌ��̋��� * �ގ��̔��ːF * ������ * �K���Ȓ����l
				color += (g_PL[pi].Color * spec) * baseSpecular * baseColor.a * 0.5;
			}
		}
	}

	// ���Ȕ���
	color += g_emissiveTex.Sample(g_ss, In.UV).rgb * g_Material.Emissive;

	// ���� �����̓������Ă��Ȃ��������^�����ɂȂ�̂�h��
	color += g_AmbientLight * baseColor.rgb * baseColor.a;

	//------------------------------------------
	// �����t�H�O
	//------------------------------------------
	if (g_DistanceFogEnable)
	{
		// �t�H�O ��1...�߂� �` 0...����
		// �}�ɃJ�[�u��������A�����t�H�O������������
		float fog = saturate(1.0 / exp(camDist * g_DistanceFogDensity));
		// �K�p
		color.rgb = lerp(g_DistanceFogColor, color.rgb, fog);
	}

	//------------------------------------------
	// �o��
	//------------------------------------------
	return float4(color, baseColor.a);
}