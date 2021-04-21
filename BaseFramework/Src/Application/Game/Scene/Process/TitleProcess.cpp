//-----------------------------------------------------------------------------
// File: TitleProcess.cpp
//
// Edit: 2021/02/23 野筒隼輔
//-----------------------------------------------------------------------------
#include "TitleProcess.h"
#include "Application/main.h"
#include "Application/Game/Scene/Scene.h"
#include "Application/Component/CameraComponent.h"
#include "Application/Manager/CameraManager.h"
#include "Application/Manager/DebugManager.h"
#include "Application/Manager/DrawManager.h"

//-----------------------------------------------------------------------------
// Name: TitleProcess()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
TitleProcess::TitleProcess()
	: m_buttonArray()
	, m_spTitleLogoTex(nullptr)
	, m_spKeyTipsTex(nullptr)
	, m_viewPosition(KdVector3(0.0f, 0.0f, 0.0f))
	, m_isPushRButton(true)
	, m_frameCount(0)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void TitleProcess::Deserialize(const json11::Json& json_object)
{
	// 共通の初期化
	GameObject::Deserialize(json_object);
	// 座標
	m_viewPosition = m_worldMatrix.GetTranslation();

	// カーソルの表示ON
	SCENE.SetCursorShow(true);
	// ぼかしをリセット
	DRAW_MAGER.SetBlurValue(Math::Vector2::Zero);

	// カメラ作成
	m_spCameraComponent = std::make_shared<CameraComponent>(*this);
	if (m_spCameraComponent) {
		CAMERA_MAGER.SetTargetCamera(m_spCameraComponent);
	}

	// ボタン設定
	ButtonSetting();

	// テクスチャ読み込み
	m_spTitleLogoTex = KdResFac.GetTexture("Data/Texture/Title/TankCombat.png");
	m_spKeyTipsTex	 = KdResFac.GetTexture("Data/Texture/Title/KeyTips.png");
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void TitleProcess::Update()
{
	// マウス情報所得
	POINT mouse_pos = {};
	GetCursorPos(&mouse_pos);
	ScreenToClient(APP.m_window.GetWndHandle(), &mouse_pos);

	// ボタン更新
	UpdateButton(mouse_pos);

	// タイトルアニメーションに戻る
	if (APP.m_window.GetWinInfo().dwWindowStatus & WS_ACTIVECAPTION) {
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
			if (!m_isPushRButton) {
				m_isPushRButton = !m_isPushRButton;
				SCENE.RequestChangeScene("Data/Text/Process/TitleAnimationProcess_01.json");
			}
		}
		else if (m_isPushRButton) { m_isPushRButton = !m_isPushRButton; }
	}

	// カメラ更新
	if (m_spCameraComponent) { m_spCameraComponent->SetCameraMatrix(m_worldMatrix); }
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void TitleProcess::DrawSprite()
{
	// タイトルロゴ
	if (m_spTitleLogoTex) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spTitleLogoTex.get(), -200, 280);
	}

	// 入力のヒント
	if (m_spKeyTipsTex) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spKeyTipsTex.get(), 420, -330);
	}

	// ボタン描画
	for (auto& button : m_buttonArray) {
		button.DrawSprite();
	}
}

//-----------------------------------------------------------------------------
// Name: ButtonSetting()
// Desc: ボタンの初期設定
//-----------------------------------------------------------------------------
void TitleProcess::ButtonSetting()
{
	std::array<std::string, 3> tex_paths;
	tex_paths.fill("");

	tex_paths = {
		"Data/Texture/Title/StartButton_N.png",
		"Data/Texture/Title/StartButton_C.png",
		"Data/Texture/Title/StartButton_C.png",
	};
	m_buttonArray[TitleButton::eStart].SetInfo(tex_paths, Math::Vector2(-880.0f, 90.0f), Math::Vector2(420.0f, 80.0f));

	tex_paths = {
		"Data/Texture/Title/OptionButton_N.png",
		"Data/Texture/Title/OptionButton_C.png",
		"Data/Texture/Title/OptionButton_C.png",
	};
	m_buttonArray[TitleButton::eOption].SetInfo(tex_paths, Math::Vector2(-880.0f, -10.0f), Math::Vector2(420.0f, 80.0f));

	tex_paths = {
		"Data/Texture/Title/CreditButton_N.png",
		"Data/Texture/Title/CreditButton_C.png",
		"Data/Texture/Title/CreditButton_C.png",
	};
	m_buttonArray[TitleButton::eCredit].SetInfo(tex_paths, Math::Vector2(-880.0f, -110.0f), Math::Vector2(420.0f, 80.0f));

	tex_paths = {
		"Data/Texture/Title/ExitButton_N.png",
		"Data/Texture/Title/ExitButton_C.png",
		"Data/Texture/Title/ExitButton_C.png",
	};
	m_buttonArray[TitleButton::eExit].SetInfo(tex_paths, Math::Vector2(-880.0f, -210.0f), Math::Vector2(420.0f, 80.0f));
}

//-----------------------------------------------------------------------------
// Name: UpdateButton()
// Desc: ボタン更新
//-----------------------------------------------------------------------------
void TitleProcess::UpdateButton(const POINT mouse_pos)
{
	//--------------------------------------------------
	// 指定のフレームで発生させる
	//--------------------------------------------------
	constexpr float start = -880.0f;
	constexpr float end = -360.0f;
	if (m_frameCount == 0) { m_buttonArray[TitleButton::eStart].SetEasing(EASING_TYPE::eExpOut, 0.2f, start, end); }
	if (m_frameCount == 3) { m_buttonArray[TitleButton::eOption].SetEasing(EASING_TYPE::eExpOut, 0.2f, start, end); }
	if (m_frameCount == 6) { m_buttonArray[TitleButton::eCredit].SetEasing(EASING_TYPE::eExpOut, 0.2f, start, end); }
	if (m_frameCount == 9) { m_buttonArray[TitleButton::eExit].SetEasing(EASING_TYPE::eExpOut, 0.2f, start, end); }
	if (m_frameCount <= 9) { m_frameCount++; }

	//--------------------------------------------------
	// 各種更新
	//--------------------------------------------------
	for (auto& button : m_buttonArray) {
		button.Update(mouse_pos);
	}

	//--------------------------------------------------
	// 押された際の処理
	//--------------------------------------------------
	// ステージ選択画面へ
	if (m_buttonArray[TitleButton::eStart].IsPush()) {
		SCENE.RequestChangeScene("Data/Text/Process/StageSelectProcess.json");
	}
	// オプション画面へ
	if (m_buttonArray[TitleButton::eOption].IsPush()) {
		SCENE.RequestChangeScene("Data/Text/Process/OptionProcess.json");
	}
	// クレジット画面へ
	if (m_buttonArray[TitleButton::eCredit].IsPush()) {
		SCENE.RequestChangeScene("Data/Text/Process/CreditProcess.json");
	}
	// 終了
	if (m_buttonArray[TitleButton::eExit].IsPush()) {
		auto result = MessageBoxA(APP.m_window.GetWndHandle(), "ゲームを終了しますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
		if (result == IDYES) { APP.End(); }
	}
}