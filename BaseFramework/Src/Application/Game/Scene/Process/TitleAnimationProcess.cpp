//-----------------------------------------------------------------------------
// File: TitleAnimationProcess.cpp
//
// Edit: 2021/03/29 野筒隼輔
//-----------------------------------------------------------------------------
#include "TitleAnimationProcess.h"
#include "Application/Game/Other/FixedCamera.h"
#include "Application/Game/Scene/Scene.h"
#include "Application/main.h"

//-----------------------------------------------------------------------------
// Name: TitleAnimationProcess()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
TitleAnimationProcess::TitleAnimationProcess()
	: m_state(ANIMATION_STATE::eScene01)
	, m_spFixedCamera(nullptr)
	, m_spLogoTexture(nullptr)
	, m_spFillBlackTexture(nullptr)
	, m_spCinemaTexture(nullptr)
	, m_whiteBack(Math::Color(0.0f, 0.0f, 0.0f, 0.0f))
	, m_frameCnt(0)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void TitleAnimationProcess::Deserialize(const json11::Json& json_object)
{
	GameObject::Deserialize(json_object);
	LoadTexture();
	SCENE.SetCursorShow(true);

	// カメラ生成
	m_spFixedCamera = std::make_shared<FixedCamera>();
	if (m_spFixedCamera && (!json_object["CameraInfo"].is_null())) {
		m_spFixedCamera->Deserialize(json_object["CameraInfo"].object_items());
	}

	//--------------------------------------------------
	// タイトルアニメーションの進行具合を設定
	//--------------------------------------------------
	if (json_object.is_null()) { return; }

	if (!json_object["State"].is_null()) {
		if (json_object["State"].string_value() == "OP01") { m_state = ANIMATION_STATE::eScene01; }
		if (json_object["State"].string_value() == "OP02") { m_state = ANIMATION_STATE::eScene02; }
		if (json_object["State"].string_value() == "OP03") { m_state = ANIMATION_STATE::eScene03; }
	}
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void TitleAnimationProcess::Update()
{
	// スキップ確認
	CheckSkipProcess();
	// カメラ更新
	UpdateCamera();
	// ブラックアウト
	UpdateBlackout();
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void TitleAnimationProcess::DrawSprite()
{
	// 上下の黒帯(映画みたいなあれ)
	if (m_spCinemaTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spCinemaTexture.get(), 0, 0);
	}

	// ブラックアウト
	if (m_spFillBlackTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spFillBlackTexture.get(), 0, 0, nullptr, &m_whiteBack);
	}

	// タイトルロゴ
	if (m_state != ANIMATION_STATE::eScene03) { return; }
	if (m_spLogoTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spLogoTexture.get(), 0, 0);
	}
}

//-----------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: テクスチャ読み込み
//-----------------------------------------------------------------------------
void TitleAnimationProcess::LoadTexture()
{
	m_spLogoTexture			= KdResFac.GetTexture("Data/Texture/TitleAnimation/TitleLogo.png");
	m_spFillBlackTexture	= KdResFac.GetTexture("Data/Texture/TitleAnimation/FillBlack.png");
	m_spCinemaTexture		= KdResFac.GetTexture("Data/Texture/TitleAnimation/Cinema.png");
}

//-----------------------------------------------------------------------------
// Name: CheckSkipProcess()
// Desc: アニメーションのスキップを確認
//-----------------------------------------------------------------------------
void TitleAnimationProcess::CheckSkipProcess()
{
	if (!(APP.m_window.GetWinInfo().dwWindowStatus & WS_ACTIVECAPTION)) { return; }

	static bool isPush = true;
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
		if (!isPush) {
			isPush = !isPush;
			SCENE.RequestChangeScene("Data/Text/Process/TitleProcess.json");
		}
	}
	else if (isPush) { isPush = !isPush; }
}

//-----------------------------------------------------------------------------
// Name: CheckChangeAnimation()
// Desc: アニメーション切り替え
//-----------------------------------------------------------------------------
void TitleAnimationProcess::CheckChangeAnimation()
{
	switch (m_state)
	{
	case ANIMATION_STATE::eScene01: SCENE.RequestChangeScene("Data/Text/Process/TitleAnimationProcess_02.json"); return;
	case ANIMATION_STATE::eScene02: SCENE.RequestChangeScene("Data/Text/Process/TitleAnimationProcess_03.json"); return;
	case ANIMATION_STATE::eScene03: SCENE.RequestChangeScene("Data/Text/Process/TitleProcess.json"); return;
	}
}

//-----------------------------------------------------------------------------
// Name: UpdateCamera()
// Desc: カメラ更新
//-----------------------------------------------------------------------------
void TitleAnimationProcess::UpdateCamera()
{
	if (m_spFixedCamera == nullptr) { return; }

	// 状態ごとに移動量確定
	KdVector3 cameraMove = KdVector3(0.0f, 0.0f, 0.0f);
	switch (m_state)
	{
	case ANIMATION_STATE::eScene01: cameraMove = KdVector3(0.00f, -0.02f, 0.00f); break;
	case ANIMATION_STATE::eScene02: cameraMove = KdVector3(0.01f,  0.00f, 0.00f); break;
	case ANIMATION_STATE::eScene03: cameraMove = KdVector3(0.00f,  0.00f, 0.02f); break;
	}

	// 移動
	m_spFixedCamera->Move(cameraMove);
	// 更新
	m_spFixedCamera->Update();
}

//-----------------------------------------------------------------------------
// Name: UpdateBlackout()
// Desc: ブラックアウト更新 ※修正.シーン遷移もやっちゃってる
//-----------------------------------------------------------------------------
void TitleAnimationProcess::UpdateBlackout()
{
	static float cnt = 0.0f;// alpha値用のcnt変数
	constexpr int BOUT_FRAME = 350;
	constexpr int CHANGE_FRAME = 420;

	m_frameCnt++;

	if (m_frameCnt > BOUT_FRAME) {
		// 徐々に黒く
		if (cnt < 1.0f) { cnt += 0.02f; }
		m_whiteBack = Math::Color(1.0f, 1.0f, 1.0f, cnt);
		// 真っ黒になったら遷移.cntリセット
		if (m_frameCnt + 1 >= CHANGE_FRAME) {
			CheckChangeAnimation();
			cnt = 0.0f;
		}
	}
}