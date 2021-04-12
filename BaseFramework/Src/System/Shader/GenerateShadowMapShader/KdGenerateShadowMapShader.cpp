//-----------------------------------------------------------------------------
// File: KdGenerateShadowMapShader.cpp
//
// Edit: 2021/02/17 �쓛����
//-----------------------------------------------------------------------------
#include "KdGenerateShadowMapShader.h"
#include "System/KdSystem.h"
#include "Application/Component/CameraComponent.h"
#include "Application/Manager/CameraManager.h"
#include "Application/Manager/DebugManager.h"

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
KdGenerateShadowMapShader::KdGenerateShadowMapShader()
	: m_VS(nullptr)
	, m_inputLayout(nullptr)
	, m_PS(nullptr)
	, m_dirLightShadowMap(nullptr)
	, m_dirLightZBuffer(nullptr)
	, m_saveRT(nullptr)
	, m_saveZ(nullptr)
	, m_numVP(1)
	, m_saveVP()
	, m_lightCameraLook(KdVector3(0.5f, -1.0f, 0.5f))
	, m_cb0()
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
bool KdGenerateShadowMapShader::Init()
{
	//-------------------------------------
	// ���_�V�F�[�_
	//-------------------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdGenerateShadowMapShader_VS.inc"

		// ���_�V�F�[�_�[�쐬
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS)))
		{
			assert(0 && "���_�V�F�[�_�[�쐬���s");
			Release();
			return false;
		}

		// �P���_�̏ڍׂȏ��
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },// ������12...R(4)G(4)B(4) * 1 = 12
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },// C++��HLSL�Ƃł̓��������Ⴄ�̂ł����������Ƃ����Ȃ���΂Ȃ�Ȃ�
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// ���_���̓��C�A�E�g�쐬
		if (FAILED(D3D.GetDev()->CreateInputLayout(&layout[0], layout.size(), &compiledBuffer[0], sizeof(compiledBuffer), &m_inputLayout)))
		{
			assert(0 && "CreateInputLayout�쐬���s");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// �s�N�Z���V�F�[�_
	//-------------------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdGenerateShadowMapShader_PS.inc"

		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS)))
		{
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// �萔�o�b�t�@�쐬
	//-------------------------------------
	m_cb0.Create();

	//-------------------------------------
	// �[�x�}�b�v��Z�o�b�t�@���쐬
	//-------------------------------------
	constexpr int size = 10240;// 1024x ���]�T�������āI
	m_dirLightShadowMap = std::make_shared<KdTexture>();
	m_dirLightShadowMap->CreateRenderTarget(size, size, DXGI_FORMAT_R32_FLOAT);
	m_dirLightZBuffer = std::make_shared<KdTexture>();
	m_dirLightZBuffer->CreateDepthStencil(size, size, DXGI_FORMAT_R32_TYPELESS);

	return true;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void KdGenerateShadowMapShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_inputLayout);
	KdSafeRelease(m_PS);
	m_cb0.Release();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void KdGenerateShadowMapShader::Begin()
{
	if ((m_saveRT) || (m_saveZ)) { return; }

	//-----------------------------------------------------
	// �L��
	//-----------------------------------------------------
	// RenderTarget Z�o�b�t�@
	D3D.GetDevContext()->OMGetRenderTargets(1, &m_saveRT, &m_saveZ);
	// �r���[�|�[�g
	D3D.GetDevContext()->RSGetViewports(&m_numVP, &m_saveVP);

	//-----------------------------------------------------
	// �ύX
	//-----------------------------------------------------
	// RenderTarget Z�o�b�t�@
	D3D.GetDevContext()->OMSetRenderTargets(1, m_dirLightShadowMap->GetRTViewAddress(), m_dirLightZBuffer->GetDSView());
	// �r���[�|�[�g
	D3D11_VIEWPORT vp = { 0, 0, static_cast<FLOAT>(m_dirLightShadowMap->GetWidth()), static_cast<FLOAT>(m_dirLightShadowMap->GetHeight()), 0, 1 };
	D3D.GetDevContext()->RSSetViewports(1, &vp);

	//-----------------------------------------------------
	// �N���A.�ݒ�
	//-----------------------------------------------------
	// �N���A
	D3D.GetDevContext()->ClearRenderTargetView(m_dirLightShadowMap->GetRTView(), Math::Color(1, 1, 1, 1));
	D3D.GetDevContext()->ClearDepthStencilView(m_dirLightZBuffer->GetDSView(), D3D11_CLEAR_DEPTH, 1, 0);

	// ���C�g�J�����ݒ�
	SettingLightCamera();
	// �V�F�[�_�[��萔�o�b�t�@���Z�b�g
	SetToDevice();
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void KdGenerateShadowMapShader::End()
{
	if ((!m_saveRT) || (!m_saveZ)) { return; }

	//-----------------------------------------------------
	// ����
	//-----------------------------------------------------
	// RenderTarget Z�o�b�t�@
	D3D.GetDevContext()->OMSetRenderTargets(1, &m_saveRT, m_saveZ);
	// �r���[�|�[�g
	D3D.GetDevContext()->RSSetViewports(1, &m_saveVP);

	//-----------------------------------------------------
	// �ۑ��p���̉��
	//-----------------------------------------------------
	KdSafeRelease(m_saveRT);
	KdSafeRelease(m_saveZ);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void KdGenerateShadowMapShader::SetToDevice()
{
	// ���_�V�F�[�_���Z�b�g
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// ���_���C�A�E�g���Z�b�g
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	// �s�N�Z���V�F�[�_���Z�b�g
	D3D.GetDevContext()->PSSetShader(m_PS, 0, 0);

	//-----------------------------------------------------
	// �萔�o�b�t�@���Z�b�g
	//-----------------------------------------------------
	// �I�u�W�F�N�g�萔�o�b�t�@
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void KdGenerateShadowMapShader::DrawMeshDepth(const KdMesh* mesh, const std::vector<KdMaterial>& materials)
{
	if (mesh == nullptr) { return; }

	// �萔�o�b�t�@��������
	m_cb0.Write();

	// ���b�V�������Z�b�g
	mesh->SetToDevice();

	// �S�T�u�Z�b�g
	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		// �ʂ��P���������ꍇ�̓X�L�b�v
		if (mesh->GetSubsets()[subi].FaceCount == 0) { continue; }

		// �}�e���A��
		const KdMaterial& material = materials[mesh->GetSubsets()[subi].MaterialNo];

		//-----------------------------------------------------
		// �e�N�X�`���Z�b�g
		//-----------------------------------------------------
		D3D.GetDevContext()->PSSetShaderResources(0, 1, material.BaseColorTex->GetSRViewAddress());

		//-----------------------------------------------------
		// �T�u�Z�b�g�`��
		//-----------------------------------------------------
		mesh->DrawSubset(subi);
	}
}

//-----------------------------------------------------------------------------
// Name: SettingLightCamera()
// Desc: ���C�g�J�����ݒ�
//-----------------------------------------------------------------------------
void KdGenerateShadowMapShader::SettingLightCamera()
{
	KdMatrix viewMat = {}, projMat = {};

	//-----------------------------------------------------
	// �J������񂩂烉�C�g���W�ɔ��f
	//-----------------------------------------------------
	std::shared_ptr<CameraComponent> camera = CAMERA_MAGER.GetTargetCamera().lock();
	KdMatrix cameraMatrix = {};
	if (camera)
	{
		const KdMatrix& cameraMat = camera->GetCameraMatrix();
		cameraMatrix.CreateTranslation(cameraMat.GetTranslation());

		KdVector3 move = cameraMat.GetAxisZ();
		move.y = 10.0f;
		move.Normalize();

		move *= 20.0f;// �J������Z����20.0f�ړ�
		cameraMatrix.Move(move);
	}

	//-----------------------------------------------------
	// ���C�g�̃r���[�s����쐬
	//-----------------------------------------------------
	// m_shadowLook�̕����Ɍ�����
	viewMat.LookTo(m_lightCameraLook, KdVector3::Up());
	// ���W�ݒ�
	KdMatrix transMat = {};
	transMat.CreateTranslation(0.0f, 0.0f, -40.0f);
	viewMat = transMat * viewMat;
	viewMat *= cameraMatrix;
	viewMat.Inverse();

	//-----------------------------------------------------
	// ���C�g�̐��ˉe�s����쐬
	//-----------------------------------------------------
	projMat.CreateProjection_Orthographic(200.0f, 200.0f, 0.0f, 300.0f);

	//-----------------------------------------------------
	// �ŏI�ݒ�
	//-----------------------------------------------------
	// �r���[*�ˉe���Z�b�g
	m_cb0.Work().mLightVP = viewMat * projMat;
	// KdModelShader�Ŏg�p���邽�߃��C�g�̒萔�o�b�t�@�ɍs������Ă���
	SHADER.m_cb8_Light.Work().DL_mLightVP = viewMat * projMat;
}