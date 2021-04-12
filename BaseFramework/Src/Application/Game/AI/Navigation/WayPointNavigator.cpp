//-----------------------------------------------------------------------------
// File: WayPointNavigator.cpp
//
// Edit: 2021/03/19 �쓛����
//-----------------------------------------------------------------------------
#include "WayPointNavigator.h"
#include "WayPoint.h"

#include "Application/Manager/DrawManager.h"

//-----------------------------------------------------------------------------
// Name: WayPointNavigator()
// Desc: �R���X�g���N�^
//-----------------------------------------------------------------------------
WayPointNavigator::WayPointNavigator()
	: m_wayPointList()
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: ������
//-----------------------------------------------------------------------------
void WayPointNavigator::Initialize()
{

}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: �X�V
//-----------------------------------------------------------------------------
void WayPointNavigator::Update()
{

}

//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: �`��
//-----------------------------------------------------------------------------
void WayPointNavigator::DrawDebug()
{
	for (auto& point : m_wayPointList) {
		if (point == nullptr) { continue; }
		KdVector3 posA = point->GetPosition();
		KdVector3 posB = posA + KdVector3(0.0f, 10.0f, 0.0f);
		DRAW_MAGER.AddDebugLine(posA, posB, kWhiteColor);
	}
}

//-----------------------------------------------------------------------------
// Name: AddWayPoint()
// Desc: �o�R�n�_�̒ǉ�
//-----------------------------------------------------------------------------
void WayPointNavigator::AddWayPoint(const KdVector3 position)
{
	std::shared_ptr<WayPoint> newPoint = std::make_shared<WayPoint>();
	if (newPoint) {
		newPoint->SetPosition(position);
		m_wayPointList.push_back(newPoint);
	}
}