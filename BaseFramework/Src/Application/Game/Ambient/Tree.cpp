//-----------------------------------------------------------------------------
// File: Tree.h
//
// Edit: 2021/03/18 �쓛����
//-----------------------------------------------------------------------------
#include "Tree.h"
#include "Application/Component/ModelComponent.h"

//-----------------------------------------------------------------------------
// Name: Tree()
// Desc: �R���X�g���N�^
//-----------------------------------------------------------------------------
Tree::Tree()
	: m_spTreeGrass(nullptr)
	, m_treeGrassLocalMatrix()
	, m_isCollapse(false)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: ������
//-----------------------------------------------------------------------------
void Tree::Deserialize(const json11::Json& json_object)
{
	GameObject::Deserialize(json_object);

	//--------------------------------------------------
	// �����_���ŉ�]�Ɗg��k��
	//--------------------------------------------------
	{
		// �[��������������A�n�[�h�E�F�A�������V�[�h�ɂ��ď�����
		static std::random_device seed_gen;
		std::mt19937 engine(seed_gen());
		// ��l�������z
		std::uniform_real_distribution<float> dist_rot(-180.0f, 180.0f);// -180�`180
		std::uniform_real_distribution<float> dist_size(0.9f, 1.2f);// 0.9�`1.2

		// ��]
		KdMatrix rotMat = {};
		rotMat.CreateRotationX(0.0f * KdToRadians);
		rotMat.RotateY(dist_rot(engine) * KdToRadians);
		m_worldMatrix = rotMat * m_worldMatrix;
		// �g��k��
		KdMatrix scaleMat = {};
		scaleMat.CreateScalling(KdVector3(dist_size(engine), dist_size(engine), dist_size(engine)));
		m_worldMatrix = scaleMat * m_worldMatrix;
	}

	//--------------------------------------------------
	// �e�q�\���̗t���� ����
	//--------------------------------------------------
	m_spTreeGrass = std::make_shared<GameObject>();
	if (m_spTreeGrass) {
		m_spTreeGrass->Deserialize(json_object["TreeGrass"].object_items());
		// �h��ON
	//	m_spTreeGrass->GetModelComponent()->SetWave(true);
		// �{�̂���̂����ݒ�
		m_treeGrassLocalMatrix.CreateTranslation(0.0f, 0.0f, 0.0f);
	}
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: �X�V
//-----------------------------------------------------------------------------
void Tree::Update()
{
	// �t���ύX�V
	if (m_spTreeGrass) { m_spTreeGrass->SetMatrix(m_treeGrassLocalMatrix * m_worldMatrix); }

	// �|��t���O�m�F
	if (!m_isCollapse) { return; }

	// ���ɗ����Ă���
	m_worldMatrix.Move(KdVector3(0.0f, 0.0f, 1.0f));
}

//-----------------------------------------------------------------------------
// Name: DrawEffect()
// Desc: Effect�`��
//-----------------------------------------------------------------------------
void Tree::DrawEffect()
{
	// �t����
	if (m_spTreeGrass) { m_spTreeGrass->GetModelComponent()->DrawEffect(); }
}

//-----------------------------------------------------------------------------
// Name: DrawShadowMap()
// Desc: �V���h�E�}�b�v�����`��
//-----------------------------------------------------------------------------
void Tree::DrawShadowMap()
{
	// ���̃V���h�E�`��
	GameObject::DrawShadowMap();
	// �t����
	if (m_spTreeGrass) { m_spTreeGrass->GetModelComponent()->DrawShadowMap(); }
}