//-----------------------------------------------------------------------------
// File: AerialBomb.h
//
// Edit: 2021/03/07 �쓛����
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class AerialBomb
// Desc: �q�󔚒e�N���X.�����@���瓊��(�͈͍U��)
//-----------------------------------------------------------------------------
class AerialBomb : public GameObject
{
public:

	// �R���X�g���N�^
	AerialBomb();
	// ������
	void Deserialize(const json11::Json& json_object) override;
	// �X�V
	void Update() override;
	// ����(����)
	void Emit(const KdVector3 pos);

private:
	std::shared_ptr<SoundInstance3D>	m_spExpSE;		// �����̌��ʉ�
	KdVector3							m_prevPosition;	// 1�t���[���O�̍��W
	KdVector3							m_axis;			// �ړ����鎲
	float								m_speed;		// �ړ����x
	float								m_gravity;		// �d��

private:
	void UpdateCollision();
	void Landing();
	void Explosion();
};