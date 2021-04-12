//-----------------------------------------------------------------------------
// File: KdModelShader.cpp
//
// Edit: 2021/03/27 �쓛����
//-----------------------------------------------------------------------------
#include "System/KdSystem.h"
#include "KdModelShader.h"

//-----------------------------------------------------------------------------
// Name: KdModelShader()
// Desc: �R���X�g���N�^
//-----------------------------------------------------------------------------
KdModelShader::KdModelShader()
	: m_VS(nullptr)
	, m_inputLayout(nullptr)
	, m_PS(nullptr)
	, m_cb0()
	, m_cb1_Material()
{
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: ������
//-----------------------------------------------------------------------------
bool KdModelShader::Init()
{
	//-------------------------------------
	// ���_�V�F�[�_
	//-------------------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdModelShader_VS.inc"

		// ���_�V�F�[�_�[�쐬
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "���_�V�F�[�_�[�쐬���s");
			Release();
			return false;
		}

		// �P���_�̏ڍׂȏ��
		// ��T���� ... �O�̗v�f�̒���Ō��݂̗v�f���`����ɂ́AD3D11_APPEND_ALIGNED_ELEMENT ���g�p����ƕ֗�
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// ���_���̓��C�A�E�g�쐬
		if (FAILED(D3D.GetDev()->CreateInputLayout(&layout[0], layout.size(), &compiledBuffer[0], sizeof(compiledBuffer), &m_inputLayout))) {
			assert(0 && "CreateInputLayout���s");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// �s�N�Z���V�F�[�_
	//-------------------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdModelShader_PS.inc"

		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// �萔�o�b�t�@�쐬
	//-------------------------------------
	m_cb0.Create();
	m_cb1_Material.Create();

	return true;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: ���
//-----------------------------------------------------------------------------
void KdModelShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_PS);
	KdSafeRelease(m_inputLayout);
	m_cb0.Release();
	m_cb1_Material.Release();
}

//-----------------------------------------------------------------------------
// Name: SetToDevice()
// Desc: GPU�ɓ]��
//-----------------------------------------------------------------------------
void KdModelShader::SetToDevice()
{
	// ���_�V�F�[�_���Z�b�g
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// ���_���C�A�E�g���Z�b�g
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);

	// �s�N�Z���V�F�[�_���Z�b�g
	D3D.GetDevContext()->PSSetShader(m_PS, 0, 0);

	// �萔�o�b�t�@���Z�b�g
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->VSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
}

//-----------------------------------------------------------------------------
// Name: DrawMesh()
// Desc: ���b�V���`��
//-----------------------------------------------------------------------------
void KdModelShader::DrawMesh(const KdMesh* mesh, const std::vector<KdMaterial>& materials)
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

		// �}�e���A���Z�b�g
		const KdMaterial& material = materials[mesh->GetSubsets()[subi].MaterialNo];

		//-----------------------
		// �}�e���A������萔�o�b�t�@�֏�������
		//-----------------------
		m_cb1_Material.Work().BaseColor = material.BaseColor;
		m_cb1_Material.Work().Emissive  = material.Emissive;
		m_cb1_Material.Work().Metallic  = material.Metallic;
		m_cb1_Material.Work().Roughness = material.Roughness;
		m_cb1_Material.Write();

		//-----------------------
		// �e�N�X�`���Z�b�g
		//-----------------------
		// BaseColor
		D3D.GetDevContext()->PSSetShaderResources(0, 1, material.BaseColorTex->GetSRViewAddress());
		// Emissive
		D3D.GetDevContext()->PSSetShaderResources(1, 1, material.EmissiveTex->GetSRViewAddress());
		// Metallic Roughness Map
		D3D.GetDevContext()->PSSetShaderResources(2, 1, material.MetallicRoughnessTex->GetSRViewAddress());
		// Normal Map
		D3D.GetDevContext()->PSSetShaderResources(3, 1, material.NormalTex->GetSRViewAddress());

		//-----------------------
		// �T�u�Z�b�g�`��
		//-----------------------
		mesh->DrawSubset(subi);
	}
}