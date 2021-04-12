//-----------------------------------------------------------------------------
// File: WayPointNavigator.h
//
// Edit: 2021/03/19 �쓛����
//-----------------------------------------------------------------------------
#pragma once

class WayPoint;

//-----------------------------------------------------------------------------
// Name: class WayPointNavigator
// Desc: WayPoint���g�p�����i�r�Q�[�V�������s��
//-----------------------------------------------------------------------------
class WayPointNavigator
{
public:

	// ������
	void Initialize();
	// �X�V
	void Update();
	// �f�o�b�O�p�̕`��
	void DrawDebug();

	//--------------------------------------------------
	// ����
	//--------------------------------------------------
	// �C���X�^���X�̏���
	static WayPointNavigator& GetInstance() { static WayPointNavigator instance; return instance; }
	// �o�R�n�_�̃��X�g������
	const std::list<std::shared_ptr<WayPoint>> GetPointList() const { return m_wayPointList; }

	//--------------------------------------------------
	// �ݒ�
	//--------------------------------------------------
	// WayPoint�ǉ�
	void AddWayPoint(const KdVector3 position);

private:
	std::list<std::shared_ptr<WayPoint>> m_wayPointList;// �Ǘ�����o�R�n�_�̃��X�g

private:
	WayPointNavigator();
};

//-----------------------------------------------------------------------------
// Define: �ȒP�ɃA�N�Z�X
//-----------------------------------------------------------------------------
#define WAYP_NAVI WayPointNavigator::GetInstance()