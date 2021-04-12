//-----------------------------------------------------------------------------
// File: KdModelShader.h
//
// Edit: 2021/03/27 �쓛����
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class KdModelShader
// Desc: ���f���`��V�F�[�_
//-----------------------------------------------------------------------------
class KdModelShader
{
public:

	// �R���X�g���N�^
	KdModelShader();
	// �f�X�g���N�^
	~KdModelShader() { Release(); }
	// ������
	bool Init();
	// ���
	void Release();
	// ���̃V�F�[�_���f�o�C�X�փZ�b�g
	void SetToDevice();
	// ���b�V���`��
	void DrawMesh(const KdMesh* mesh, const std::vector<KdMaterial>& materials);

	//--------------------------------------------------
	// �ݒ�
	//--------------------------------------------------
	// ���[���h�s��Z�b�g
	void SetWorldMatrix(const Math::Matrix& m) { m_cb0.Work().mW = m; }
	// UV�̃I�t�Z�b�g�ݒ�
	void SetUvOffset(const Math::Vector2& offset) { m_cb0.Work().UVOffset = offset; }

private:
	ID3D11VertexShader*	m_VS;			// ���_�V�F�[�_�[
	ID3D11InputLayout*	m_inputLayout;	// ���_���̓��C�A�E�g
	ID3D11PixelShader*	m_PS;			// �s�N�Z���V�F�[�_�[


	// �萔�o�b�t�@(�I�u�W�F�N�g�P�ʍX�V)
	struct cbObject
	{
		Math::Matrix	mW; // ���[���h�s��
		Math::Vector2	UVOffset = { 0.0f,0.0f };
		float			tmp[2];
	};
	KdConstantBuffer<cbObject> m_cb0;

	// �萔�o�b�t�@(�}�e���A���P�ʍX�V)
	struct cbMaterial
	{
		Math::Vector4	BaseColor;
		Math::Vector3	Emissive;
		float			Metallic;
		float			Roughness;
		float			tmp[3];
	};
	KdConstantBuffer<cbMaterial> m_cb1_Material;
};