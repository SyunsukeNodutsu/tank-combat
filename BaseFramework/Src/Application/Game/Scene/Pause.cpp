//-----------------------------------------------------------------------------
// File: Pause.cpp
//
// Edit: 2021/03/18 野筒隼輔
//-----------------------------------------------------------------------------
#include "Pause.h"
#include "Scene.h"
#include "Application/main.h"
#include "Application/Manager/ClearManager.h"

//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
Pause::Pause()
	: m_spState(nullptr)
	, m_spBackTexture(nullptr)
	, m_tmpVolume(0.0f)
	, m_isEnable(false)
	, m_isPlaying(false)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void Pause::Deserialize()
{
	LoadTexture();

	// 状態.命令状態から
	m_spState = std::make_shared<StateCommand>();
	if (m_spState) { m_spState->Deserialize(*this); }
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void Pause::Update()
{
	if (SCENE.IsEdiorMode()) { return; }

	// 切り替え確認 ※有効無効にかかわらず確認
	CheckChangeEnable();

	if (!m_isEnable) { return; }

	// マウス座標所得
	/*
	* 修正
	* GetDesktopMousePos
	* GetClientMousePos みたいな感じでクラスにまとめたほうがいい
	*/
	POINT mouse_pos = {};
	GetCursorPos(&mouse_pos);
	ScreenToClient(APP.m_window.GetWndHandle(), &mouse_pos);

	// 状態ごとの更新
	if (m_spState) { m_spState->Update(*this, mouse_pos); }
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void Pause::DrawSprite()
{
	if (!m_isEnable) { return; }

	// 背景
	if (m_spBackTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spBackTexture.get(), 0, 0);
	}

	// 目的
	if (m_spMissionTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spMissionTexture.get(), 360, 20);
	}

	// 説明
	if (m_spTipsTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spTipsTexture.get(), 0, -300);
	}

	// 状態ごとの2D描画
	if (m_spState) { m_spState->DrawSprite(*this); }
}

//-----------------------------------------------------------------------------
// Name: CheckChangeEnable()
// Desc: ポーズ画面切り替えの確認
//-----------------------------------------------------------------------------
void Pause::CheckChangeEnable()
{
	// ゲーム中のみ
	if (!m_isPlaying) { return; }
	// ゲームが終了しているか
	if (CLEAR_MAGER.IsGameEnd()) { return; }
	// アクティブか確認
	if (!(APP.m_window.GetWinInfo().dwWindowStatus & WS_ACTIVECAPTION)) { return; }

	static bool isPush = true;
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
		if (!isPush) {
			isPush = !isPush;
			m_isEnable = !m_isEnable;
			SCENE.SetCursorShow(m_isEnable);

			// 音量を下げる ※修正.AUDIOに実装
			if (m_isEnable) {
				m_tmpVolume = AUDIO.GetUserSettingVolume();
				AUDIO.SetUserSettingVolume(AUDIO.GetUserSettingVolume() * 0.4f);
				// 
				if (m_spState) { m_spState->Deserialize(*this); }
			}
			// 音量を元に戻す
			else {
				AUDIO.SetUserSettingVolume(m_tmpVolume);

				// マウス座標を中央に戻してから再開 ... 急にカメラが動くのを予防(修正.クラスにまとめる)
				{
					// 停止させる座標
					RECT rect = {};
					SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
					const int stopX = (rect.right - rect.left) / 2;
					const int stopY = (rect.bottom - rect.top) / 2;

					// 画面外に出ないように固定
					SetCursorPos(stopX, stopY);
				}
			}
		}
	}
	else if (isPush) { isPush = !isPush; }
}

//-----------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: テクスチャ読み込み
//-----------------------------------------------------------------------------
void Pause::LoadTexture()
{
	m_spBackTexture		= KdResFac.GetTexture("Data/Texture/Pause/Back.png");
	m_spMissionTexture	= KdResFac.GetTexture("Data/Texture/Pause/Mission/Stage01.png.png");
	m_spTipsTexture		= KdResFac.GetTexture("Data/Texture/Pause/Tips/Order.png");
}



//=============================================================================
// 
// StateCommand
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: StateCommand()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
Pause::StateCommand::StateCommand()
	: m_spState(nullptr)
	, m_buttonArray()
	, m_stateOrder(true)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void Pause::StateCommand::Deserialize(Pause& owner)
{
	m_spState = std::make_shared<StateOrder>();
	if (m_spState) { m_spState->Initialize(owner); }

	m_stateOrder = true;

	std::array<std::string, 3> tex_paths;
	tex_paths.fill("");

	tex_paths = {
		"Data/Texture/Pause/Button/OrderButton_N.png",
		"Data/Texture/Pause/Button/OrderButton_C.png",
		"Data/Texture/Pause/Button/OrderButton_C.png",
	};
	m_buttonArray[0].SetInfo(tex_paths, Math::Vector2(-540.0f, 310.0f), Math::Vector2(166.0f, 78.0f));

	tex_paths = {
		"Data/Texture/Pause/Button/SystemButton_N.png",
		"Data/Texture/Pause/Button/SystemButton_C.png",
		"Data/Texture/Pause/Button/SystemButton_C.png",
	};
	m_buttonArray[1].SetInfo(tex_paths, Math::Vector2(-360.0f, 310.0f), Math::Vector2(166.0f, 78.0f));
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void Pause::StateCommand::Update(Pause& owner, const POINT mouse_pos)
{
	// 状態ごとの更新
	if (m_spState) { m_spState->Update(owner, mouse_pos); }

	//--------------------------------------------------
	// 各種更新
	//--------------------------------------------------
	for (auto& button : m_buttonArray) {
		button.Update(mouse_pos);
	}

	//--------------------------------------------------
	// 選択された際の処理
	//--------------------------------------------------
	// Order
	if (m_buttonArray[0].IsSelect()) {
		// 前フレームの状態を比較して変化していたら
		if (!m_stateOrder) {
			m_spState = std::make_shared<StateOrder>();
			m_spState->Initialize(owner);
			m_stateOrder = true;
		}
	}
	// System
	if (m_buttonArray[1].IsSelect()) {
		if (m_stateOrder) {
			m_spState = std::make_shared<StateSystem>();
			m_spState->Initialize(owner);
			m_stateOrder = false;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void Pause::StateCommand::DrawSprite(Pause& owner)
{
	// 状態ごとの2D描画
	if (m_spState) { m_spState->DrawSprite(owner); }

	// ボタン2D描画
	for (auto& button : m_buttonArray) {
		button.DrawSprite();
	}
}