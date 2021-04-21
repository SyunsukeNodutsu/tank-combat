//-----------------------------------------------------------------------------
// File: StageSelectProcess.h
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#include "StageSelectProcess.h"
#include "Application/main.h"
#include "Application/Game/Scene/Scene.h"
#include "Application/Manager/DebugManager.h"

//-----------------------------------------------------------------------------
// Name: StageSelectProcess()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
StageSelectProcess::StageSelectProcess()
	: m_selectStage(SelectStage::eNone)
	, m_prevSelectStage(SelectStage::eNone)
	, m_buttonArray()
	, m_spBackTexture(nullptr)
	, m_spKeyTipsTexture(nullptr)
	, m_spLogoTexture(nullptr)
	, m_frameCount(0)
	, m_easingMapHeight()
	, m_easingMapWidth()
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void StageSelectProcess::Deserialize(const json11::Json& json_object)
{
	GameObject::Deserialize(json_object);

	// テクスチャ読み込み
	LoadTexture();
	// ボタン設定
	SettingButton();
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void StageSelectProcess::Update()
{
	// 遷移
	if (APP.m_window.GetWinInfo().dwWindowStatus & WS_ACTIVECAPTION) {
		static bool isPush = true;
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
			if (!isPush) {
				isPush = !isPush;
				AUDIO.Play("Data/Audio/SE/Cancel.wav", 0.5f);
				SCENE.RequestChangeScene("Data/Text/Process/TitleProcess.json");
			}
		}
		else if (isPush) { isPush = !isPush; }
	}

	// マウス情報所得
	POINT mouse_pos = {};
	GetCursorPos(&mouse_pos);
	ScreenToClient(APP.m_window.GetWndHandle(), &mouse_pos);

	// ボタン更新
	UpdateButton(mouse_pos);
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void StageSelectProcess::DrawSprite()
{
	// 背景
	if (m_spBackTexture) {
		KdMatrix scaleMat = {};
		scaleMat.CreateScalling(KdVector3(1.5f, 1.5f, 1.5f));
		scaleMat.Move(CheckBackTexPos(m_selectStage));
		SHADER.m_spriteShader.SetMatrix(scaleMat);
		SHADER.m_spriteShader.DrawTex(m_spBackTexture.get(), 0, 0);
	}

	// 入力のヒント
	if (m_spKeyTipsTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spKeyTipsTexture.get(), -420, -330);
	}

	// ロゴ
	if (m_spLogoTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spLogoTexture.get(), -360, 300);
	}

	// ステージの情報
	std::shared_ptr<KdTexture> texture = nullptr;
	switch (m_selectStage)
	{
	case SelectStage::eNone:	texture = KdResFac.GetTexture("Data/Texture/StageSelect/Stage00_Info.png"); break;
	case SelectStage::e01:		texture = KdResFac.GetTexture("Data/Texture/StageSelect/Stage01_Info.png"); break;
	case SelectStage::e02:		texture = KdResFac.GetTexture("Data/Texture/StageSelect/Stage02_Info.png"); break;
	case SelectStage::e03:		texture = KdResFac.GetTexture("Data/Texture/StageSelect/Stage03_Info.png"); break;
	case SelectStage::e04:		texture = KdResFac.GetTexture("Data/Texture/StageSelect/Stage04_Info.png"); break;
	}
	if (texture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(texture.get(), 420, -220, nullptr, &kTranslucent2);
	}

	// ボタン描画
	for (auto& button : m_buttonArray) {
		button.DrawSprite();
	}
}

//-----------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: テクスチャ読み込み
//-----------------------------------------------------------------------------
void StageSelectProcess::LoadTexture()
{
	m_spBackTexture		= KdResFac.GetTexture("Data/Texture/StageSelect/Back.png");
	m_spKeyTipsTexture	= KdResFac.GetTexture("Data/Texture/StageSelect/KeyTips.png");
	m_spLogoTexture		= KdResFac.GetTexture("Data/Texture/StageSelect/Logo.png");
}

//-----------------------------------------------------------------------------
// Name: SettingButton()
// Desc: ボタン初期設定
//-----------------------------------------------------------------------------
void StageSelectProcess::SettingButton()
{
	std::array<std::string, 3> tex_paths = {};
	constexpr Math::Vector2 size = Math::Vector2(304.0f, 40.0f);
	constexpr Math::Vector2 pos = Math::Vector2(-460.0f, 360.0f);

	tex_paths = {
		"Data/Texture/StageSelect/button01_N.png",
		"Data/Texture/StageSelect/button01_S.png",
		"Data/Texture/StageSelect/button01_S.png",
	};
	m_buttonArray[0].SetInfo(tex_paths, pos, size);

	tex_paths = {
		"Data/Texture/StageSelect/button02_N.png",
		"Data/Texture/StageSelect/button02_S.png",
		"Data/Texture/StageSelect/button02_S.png",
	};
	m_buttonArray[1].SetInfo(tex_paths, pos, size);

	tex_paths = {
		"Data/Texture/StageSelect/button03_N.png",
		"Data/Texture/StageSelect/button03_S.png",
		"Data/Texture/StageSelect/button03_S.png",
	};
	m_buttonArray[2].SetInfo(tex_paths, pos, size);

	tex_paths = {
		"Data/Texture/StageSelect/button04_N.png",
		"Data/Texture/StageSelect/button04_S.png",
		"Data/Texture/StageSelect/button04_S.png",
	};
	m_buttonArray[3].SetInfo(tex_paths, pos, size);
}

//-----------------------------------------------------------------------------
// Name: UpdateButton()
// Desc: ボタン更新
//-----------------------------------------------------------------------------
void StageSelectProcess::UpdateButton(const POINT mouse_pos)
{
	// 1フレーム前の状態を記憶
	m_prevSelectStage = m_selectStage;

	//--------------------------------------------------
	// 指定のフレームで発生させる
	//--------------------------------------------------
	constexpr float start = 360.0f;
	if (m_frameCount == 0) { m_buttonArray[0].SetEasing(EASING_TYPE::eExpOut, 0.2f, start, 160, false); }
	if (m_frameCount == 3) { m_buttonArray[1].SetEasing(EASING_TYPE::eExpOut, 0.2f, start, 100, false); }
	if (m_frameCount == 6) { m_buttonArray[2].SetEasing(EASING_TYPE::eExpOut, 0.2f, start,  40, false); }
	if (m_frameCount == 9) { m_buttonArray[3].SetEasing(EASING_TYPE::eExpOut, 0.2f, start, -20, false); }
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
	if (m_buttonArray[0].IsPush()) {
		SCENE.RequestChangeScene("Data/Text/Process/Stage_01Process.json");
	}
	if (m_buttonArray[1].IsPush()) {
		SCENE.RequestChangeScene("Data/Text/Process/Stage_02Process.json");
	}
	if (m_buttonArray[2].IsPush()) {
		SCENE.RequestChangeScene("Data/Text/Process/Stage_03Process.json");
	}
	if (m_buttonArray[3].IsPush()) {
		SCENE.RequestChangeScene("Data/Text/Process/Stage_04Process.json");
	}

	//--------------------------------------------------
	// 選択された際の処理 ※押されはしない
	//--------------------------------------------------
	if (m_buttonArray[0].IsSelect()) { if (SelectStage::e01 != m_prevSelectStage) { m_selectStage = SelectStage::e01; return; } }
	if (m_buttonArray[1].IsSelect()) { if (SelectStage::e02 != m_prevSelectStage) { m_selectStage = SelectStage::e02; return; } }
	if (m_buttonArray[2].IsSelect()) { if (SelectStage::e03 != m_prevSelectStage) { m_selectStage = SelectStage::e03; return; } }
	if (m_buttonArray[3].IsSelect()) { if (SelectStage::e04 != m_prevSelectStage) { m_selectStage = SelectStage::e04; return; } }
}

//-----------------------------------------------------------------------------
// Name: CheckBackTexPos()
// Desc: 背景の移動を現在のボタンの状態から確認
//-----------------------------------------------------------------------------
const KdVector3 StageSelectProcess::CheckBackTexPos(const SelectStage select)
{
	switch (select)
	{
	case SelectStage::e01: return KdVector3(-170.0f,  -50.0f, 0.0f); break;
	case SelectStage::e02: return KdVector3(-110.0f,   54.0f, 0.0f); break;
	case SelectStage::e03: return KdVector3(-688.0f, -215.0f, 0.0f); break;
	case SelectStage::e04: return KdVector3( 425.0f,  265.0f, 0.0f); break;
	}
	return KdVector3(0.0f, 0.0f, 0.0f);
}