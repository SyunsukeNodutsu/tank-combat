//-----------------------------------------------------------------------------
// File: WayPointNavigator.h
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

class WayPoint;

//-----------------------------------------------------------------------------
// Name: class WayPointNavigator
// Desc: WayPointを使用したナビゲーションを行う
//-----------------------------------------------------------------------------
class WayPointNavigator
{
public:

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// デバッグ用の描画
	void DrawDebug();

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// インスタンスの所得
	static WayPointNavigator& GetInstance() { static WayPointNavigator instance; return instance; }
	// 経由地点のリストを所得
	const std::list<std::shared_ptr<WayPoint>> GetPointList() const { return m_wayPointList; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// WayPoint追加
	void AddWayPoint(const KdVector3 position);

private:
	std::list<std::shared_ptr<WayPoint>> m_wayPointList;// 管理する経由地点のリスト

private:
	WayPointNavigator();
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define WAYP_NAVI WayPointNavigator::GetInstance()