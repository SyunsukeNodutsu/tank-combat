//-----------------------------------------------------------------------------
// File: KdGenerateShadowMapShader.h
//
// Edit: 2021/02/17 �쓛����
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class KdGenerateShadowMapShader
// Desc: �V���h�E�}�b�v(�[�x�}�b�v)�����`��V�F�[�_�[
//-----------------------------------------------------------------------------
class KdGenerateShadowMapShader
{
public:

	// �R���X�g���N�^
	KdGenerateShadowMapShader();

	// �f�X�g���N�^
	~KdGenerateShadowMapShader() { Release(); }

	// ������
	bool Init();

	// ���
	void Release();

	// �V���h�E�}�b�v�����`��J�n
	void Begin();

	// �V���h�E�}�b�v�����`��I��
	void End();

	// ���̃V�F�[�_���f�o�C�X�փZ�b�g
	void SetToDevice();

	// ���b�V���`��
	void DrawMeshDepth(const KdMesh* mesh, const std::vector<KdMaterial>& materials);

	//--------------------------------------------------
	// �ݒ�.����
	//--------------------------------------------------
	// ���s���p�[�x�}�b�v�擾
	std::shared_ptr<KdTexture> GetDirShadowMap() const { return m_dirLightShadowMap; }
	// ���[���h�s��Z�b�g
	void SetWorldMatrix(const Math::Matrix& m) { m_cb0.Work().mW = m; }

private:
	ID3D11VertexShader*				m_VS;					// ���_�V�F�[�_�[
	ID3D11InputLayout*				m_inputLayout;			// ���_���̓��C�A�E�g
	ID3D11PixelShader*				m_PS;					// �s�N�Z���V�F�[�_�[
	std::shared_ptr<KdTexture>		m_dirLightShadowMap;	// ���s���p �[�x�}�b�v
	std::shared_ptr<KdTexture>		m_dirLightZBuffer;		// ���s���p Z�o�b�t�@
	// �L���p
	ID3D11RenderTargetView*			m_saveRT;				// �L���p.RenderTarget
	ID3D11DepthStencilView*			m_saveZ;				// �L���p.Z�o�b�t�@
	UINT							m_numVP;
	D3D11_VIEWPORT					m_saveVP;				// �L���p.�r���[�|�[�g
	// �I�v�V����
	KdVector3						m_lightCameraLook;		// ���C�g�J�����̌���

	// �萔�o�b�t�@
	struct cbObject
	{
		KdMatrix mW;			// ���[���h�s��
		KdMatrix mLightVP;		// ���C�g�J�����̃r���[�s�� * �ˉe�s��
	};
	KdConstantBuffer<cbObject> m_cb0;

private:
	void SettingLightCamera();
};
