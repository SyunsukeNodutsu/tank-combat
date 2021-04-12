//-----------------------------------------------------------------------------
// File: PauseCommand.cpp
//
// Edit: 2021/04/06 野筒隼輔
//-----------------------------------------------------------------------------
#include "Pause.h"
#include "Scene.h"
#include "Application/Manager/DrawManager.h"
#include "Application/Manager/ObjectManager.h"

//=============================================================================
// 
// StateOrder
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: StateOrder()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
Pause::StateCommand::StateOrder::StateOrder()
	: m_buttonArray()
	, m_frameCount(0)
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void Pause::StateCommand::StateOrder::Initialize(Pause& owner)
{
	std::array<std::string, 3> tex_paths;
	tex_paths.fill("");

	tex_paths = {
		"Data/Texture/Pause/Button/TankOrder_N.png",
		"Data/Texture/Pause/Button/TankOrder_C.png",
		"Data/Texture/Pause/Button/TankOrder_C.png",
	};
	m_buttonArray[0].SetInfo(tex_paths, Math::Vector2(-770.0f, 100.0f), Math::Vector2(260.0f, 78.0f));

	tex_paths = {
		"Data/Texture/Pause/Button/BombingOrder_N.png",
		"Data/Texture/Pause/Button/BombingOrder_C.png",
		"Data/Texture/Pause/Button/BombingOrder_C.png",
	};
	m_buttonArray[1].SetInfo(tex_paths, Math::Vector2(-770.0f, -20.0f), Math::Vector2(260.0f, 78.0f));
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void Pause::StateCommand::StateOrder::Update(Pause& owner, const POINT& mouse_pos)
{
	//--------------------------------------------------
	// 指定フレームで出現
	//--------------------------------------------------
	constexpr float start = -770.0f;
	constexpr float end = -400.0f;
	if (m_frameCount ==  0) { m_buttonArray[0].SetEasing(EASING_TYPE::eExpOut, 0.2f, start, end); }
	if (m_frameCount ==  6) { m_buttonArray[1].SetEasing(EASING_TYPE::eExpOut, 0.2f, start, end); }
	if (m_frameCount <= 12) { m_frameCount++; }

	//--------------------------------------------------
	// 各種更新
	//--------------------------------------------------
	for (auto& button : m_buttonArray) {
		button.Update(mouse_pos);
	}

	//--------------------------------------------------
	// 選択された際の処理
	//--------------------------------------------------
	// 戦車
	if (m_buttonArray[0].IsPush()) {

	}
	// 航空
	if (m_buttonArray[1].IsPush()) {

	}
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void Pause::StateCommand::StateOrder::DrawSprite(Pause& owner)
{
	for (auto& button : m_buttonArray) {
		button.DrawSprite();
	}
}



//=============================================================================
// 
// StateCommand
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: StateSystem()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
Pause::StateCommand::StateSystem::StateSystem()
	: m_buttonArray()
	, m_frameCount(0)
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void Pause::StateCommand::StateSystem::Initialize(Pause& owner)
{
	std::array<std::string, 3> tex_paths;
	tex_paths.fill("");

	tex_paths = {
		"Data/Texture/Pause/Button/Option_N.png",
		"Data/Texture/Pause/Button/Option_C.png",
		"Data/Texture/Pause/Button/Option_C.png",
	};
	m_buttonArray[0].SetInfo(tex_paths, Math::Vector2(-770.0f, 100.0f), Math::Vector2(260.0f, 78.0f));

	tex_paths = {
		"Data/Texture/Pause/Button/Restart_N.png",
		"Data/Texture/Pause/Button/Restart_C.png",
		"Data/Texture/Pause/Button/Restart_C.png",
	};
	m_buttonArray[1].SetInfo(tex_paths, Math::Vector2(-770.0f, -20.0f), Math::Vector2(260.0f, 78.0f));

	tex_paths = {
		"Data/Texture/Pause/Button/ExitStage_N.png",
		"Data/Texture/Pause/Button/ExitStage_C.png",
		"Data/Texture/Pause/Button/ExitStage_C.png",
	};
	m_buttonArray[2].SetInfo(tex_paths, Math::Vector2(-770.0f, -140.0f), Math::Vector2(260.0f, 78.0f));
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void Pause::StateCommand::StateSystem::Update(Pause& owner, const POINT& mouse_pos)
{
	//--------------------------------------------------
	// 指定フレームで出現
	//--------------------------------------------------
	constexpr float start = -770.0f;
	constexpr float end = -400.0f;
	if (m_frameCount ==  0) { m_buttonArray[0].SetEasing(EASING_TYPE::eExpOut, 0.2f, start, end); }
	if (m_frameCount ==  6) { m_buttonArray[1].SetEasing(EASING_TYPE::eExpOut, 0.2f, start, end); }
	if (m_frameCount == 12) { m_buttonArray[2].SetEasing(EASING_TYPE::eExpOut, 0.2f, start, end); }
	if (m_frameCount <= 18) { m_frameCount++; }

	//--------------------------------------------------
	// 各種更新
	//--------------------------------------------------
	// ボタン更新
	for (auto& button : m_buttonArray) {
		button.Update(mouse_pos);
	}

	//--------------------------------------------------
	// 選択された際の処理
	//--------------------------------------------------
	// Option
	if (m_buttonArray[0].IsPush()) {
		return;
	}
	// Restart
	if (m_buttonArray[1].IsPush()) {
		owner.m_isEnable = false;
		DRAW_MAGER.SetBlurValue(Math::Vector2::Zero);
		OBJ_MAGER.Reset();
		// BGM停止.音量を元に戻す
		AUDIO.AllStop();
		AUDIO.SetUserSettingVolume(owner.m_tmpVolume);
		// マウスカーソルを中央に.遷移
		SCENE.SetCenterCursor();
		SCENE.RequestChangeScene(SCENE.GetNowSceneFilepath());
		return;
	}
	// ExitGame
	if (m_buttonArray[2].IsPush()) {
		owner.m_isEnable = owner.m_isPlaying = false;
		DRAW_MAGER.SetBlurValue(Math::Vector2::Zero);
		OBJ_MAGER.Reset();
		// BGM停止.音量を元に戻す
		AUDIO.AllStop();
		AUDIO.SetUserSettingVolume(owner.m_tmpVolume);
		// マウスカーソルを中央に.遷移
		SCENE.SetCenterCursor();
		SCENE.RequestChangeScene("Data/Text/Process/StageSelectProcess.json");
		return;
	}
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void Pause::StateCommand::StateSystem::DrawSprite(Pause& owner)
{
	// ボタン描画
	for (auto& button : m_buttonArray) {
		button.DrawSprite();
	}
}