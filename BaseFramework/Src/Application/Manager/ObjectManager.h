//-----------------------------------------------------------------------------
// File: ObjectManager.h
//
// Edit: 2021/02/11 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
class Tank;

//-----------------------------------------------------------------------------
// Name: class ObjectManager
// Desc: ゲームに登場するObjectの管理
//-----------------------------------------------------------------------------
class ObjectManager
{
public:

	// リセット
	void Reset();
	// 更新
	void Update();

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// コンテナに追加 ※TankはTAGで確認後.追加
	void AddObject(const std::shared_ptr<GameObject> object);

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// インスタンス所得
	static ObjectManager& GetInstance() { static ObjectManager instance; return instance; }
	// 引数の名前と合致する最初のObjectを所得
	const std::shared_ptr<GameObject> SearchObject(const std::string& a_name);
	// Objectコンテナ所得
	const std::list<std::shared_ptr<GameObject>> GetObjectList() const { return m_spObjectList; }
	// Tankコンテナ所得
	const std::list<std::shared_ptr<Tank>> GetTankList() const { return m_spTankList; }

private:
	std::list<std::shared_ptr<GameObject>>	m_spObjectList;	// 登場するObjectのコンテナ
	std::list<std::shared_ptr<Tank>>		m_spTankList;	// 登場するTankのコンテナ

private:
	ObjectManager();
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define OBJ_MAGER ObjectManager::GetInstance()