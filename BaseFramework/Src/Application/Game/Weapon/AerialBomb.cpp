//-----------------------------------------------------------------------------
// File: AerialBomb.cpp
//
// Edit: 2021/03/07 �쓛����
//-----------------------------------------------------------------------------
#include "AerialBomb.h"
#include "Application/Game/AI/Character/AutomaticTank.h"
#include "Application/Game/Player/PlayerTank.h"
#include "Application/Game/Other/AnimationEffect.h"
#include "Application/Manager/ObjectManager.h"
#include "Application/Component/CollisionComponent.h"

//-----------------------------------------------------------------------------
// Name: AerialBomb()
// Desc: �R���X�g���N�^
//-----------------------------------------------------------------------------
AerialBomb::AerialBomb()
	: m_spExpSE(nullptr)
	, m_prevPosition(KdVector3(0.0f, 0.0f, 0.0f))
	, m_axis(KdVector3(0.0f, -1.0f, 0.0f))
	, m_speed(0.6f)
	, m_gravity(0.006f)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: ������
//-----------------------------------------------------------------------------
void AerialBomb::Deserialize(const json11::Json& json_object)
{
	// ���N���X�̏�����(���ʂ̏�����)
	GameObject::Deserialize(json_object);

	// ���ʉ��ǂݍ���
	m_spExpSE = AUDIO.GetSoundInstance3D("Data/Audio/SE/Explosion01.wav");
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: �X�V
//-----------------------------------------------------------------------------
void AerialBomb::Update()
{
	// �ړ��O�̍��W���o����
	m_prevPosition = m_worldMatrix.GetTranslation();

	// �ړ��ݒ�
	m_axis.Normalize();
	m_axis *= m_speed;// �����e�̑��x�͌��\�������

	// �ړ�(����)
	m_axis.y -= m_gravity;
	m_worldMatrix.Move(m_axis);

	// ����
	UpdateCollision();
}

//-----------------------------------------------------------------------------
// Name: Emit()
// Desc: ����
//-----------------------------------------------------------------------------
void AerialBomb::Emit(const KdVector3 a_pos)
{
	// ���W��ݒ�
	m_worldMatrix.SetTranslation(a_pos);
}

//-----------------------------------------------------------------------------
// Name: UpdateCollision()
// Desc: �Փ˔���
//-----------------------------------------------------------------------------
void AerialBomb::UpdateCollision()
{
	//--------------------------------------------------
	// �Փ˔���p�f�[�^�쐬
	//--------------------------------------------------

	// ���C����p
	RayInfo rayInfo = {};
	rayInfo.m_pos = m_worldMatrix.GetTranslation();
	rayInfo.m_dir = m_worldMatrix.GetTranslation() - m_prevPosition;// 1�t���[���̈ړ����̂ݔ���
	rayInfo.m_maxRange = rayInfo.m_dir.Length();
	rayInfo.m_dir.Normalize();

	// ���C����̌��ʂ��󂯎��
	KdRayResult rayResult;

	//--------------------------------------------------
	// �SGameObject�Ɣ���
	//--------------------------------------------------
	for (auto& object : OBJ_MAGER.GetObjectList()) {
		// TAG�m�F.StageObjec�݂̂Ɣ���
		if (object->GetTag() & OBJECT_TAG::TAG_StageObject) {
			if (!object->GetCollisionComponent()) { continue; }
			if (object->GetCollisionComponent()->HitCheckByRay(rayInfo, rayResult) == false) { continue; }
			// ���e
			Landing();
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Landing()
// Desc: ���e
//-----------------------------------------------------------------------------
void AerialBomb::Landing()
{
	// ����
	Explosion();

	//--------------------------------------------------
	// �����GameObject�ɒʒm�𑗂�
	//--------------------------------------------------
	for (auto& object : OBJ_MAGER.GetObjectList()) {
		if (object.get() == this) { continue; }

		// TAG�m�F ... ���
		if (object->GetTag() & OBJECT_TAG::TAG_Tank) {
			// Tank�N���X�Ƀ_�E���L���X�g���Ēʒm�𑗂�
			std::shared_ptr<Tank> tank = std::dynamic_pointer_cast<Tank>(object);
			if (tank) { tank->NoticeAerialBombLanding(m_worldMatrix.GetTranslation()); }
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Explosion()
// Desc: ����
//-----------------------------------------------------------------------------
void AerialBomb::Explosion()
{
	// Effect
	KdVector3 centerPos = m_worldMatrix.GetTranslation();
	EFFEKSEER.Play(u"Data/Effekseer/Explosion.efk", KdVector3(centerPos.x + 10, 0.0f, centerPos.z));
	EFFEKSEER.Play(u"Data/Effekseer/Explosion.efk", KdVector3(centerPos.x - 10, 0.0f, centerPos.z));
	EFFEKSEER.Play(u"Data/Effekseer/Explosion.efk", KdVector3(centerPos.x, 0.0f, centerPos.z + 10));
	EFFEKSEER.Play(u"Data/Effekseer/Explosion.efk", KdVector3(centerPos.x, 0.0f, centerPos.z - 10));

	// ���ʉ�
	if (m_spExpSE) {
		m_spExpSE->SetVolume(6.0f);
		m_spExpSE->Play();
		m_spExpSE->SetPosition(centerPos);
	}

	// ����
	m_isEnable = false;
}