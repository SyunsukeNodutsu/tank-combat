//-----------------------------------------------------------------------------
// File: WayPointNavigator.cpp
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#include "WayPointNavigator.h"
#include "WayPoint.h"

#include "Application/Manager/DrawManager.h"

//-----------------------------------------------------------------------------
// Name: WayPointNavigator()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
WayPointNavigator::WayPointNavigator()
	: m_wayPointList()
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void WayPointNavigator::Initialize()
{

}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void WayPointNavigator::Update()
{

}

//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: 描画
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
// Desc: 経由地点の追加
//-----------------------------------------------------------------------------
void WayPointNavigator::AddWayPoint(const KdVector3 position)
{
	std::shared_ptr<WayPoint> newPoint = std::make_shared<WayPoint>();
	if (newPoint) {
		newPoint->SetPosition(position);
		m_wayPointList.push_back(newPoint);
	}
}