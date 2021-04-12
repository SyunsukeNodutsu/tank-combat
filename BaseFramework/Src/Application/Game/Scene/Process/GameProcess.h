//-----------------------------------------------------------------------------
// File: GameProcess.h
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class GameProcess
// Desc: GameSceneの管理者をカテゴリ分け
//-----------------------------------------------------------------------------
class GameProcess : public GameObject
{
public:

	// コンストラクタ
	GameProcess() : m_isPushEnter(true) {}

protected:
	bool m_isPushEnter;
};