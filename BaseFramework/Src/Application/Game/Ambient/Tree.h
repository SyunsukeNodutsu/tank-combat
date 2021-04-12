//-----------------------------------------------------------------------------
// File: Tree.h
//
// Edit: 2021/03/18 �쓛����
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class Tree
// Desc: �؃N���X �t���ς̐�����.�w�ʃJ�����O�����Ȃ����ߋ��Draw�֐�������
//-----------------------------------------------------------------------------
class Tree : public GameObject
{
public:

	// �R���X�g���N�^
	Tree();
	// ������
	void Deserialize(const json11::Json& json_object);
	// �X�V
	void Update() override;
	// Effect�`��
	void DrawEffect() override;
	// �V���h�E�}�b�v�`��
	void DrawShadowMap() override;

	//--------------------------------------------------
	// �ݒ�
	//--------------------------------------------------
	// �|�󂳂���
	inline void Collapse() { m_isCollapse = true; }

private:
	std::shared_ptr<GameObject> m_spTreeGrass;			// �؂̗t����
	KdMatrix					m_treeGrassLocalMatrix;	// �؂̗t���ς̃��[�J���s��
	bool						m_isCollapse;			// �|�󂷂邩�ǂ���
};