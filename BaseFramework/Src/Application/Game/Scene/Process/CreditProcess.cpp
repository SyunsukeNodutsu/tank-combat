//-----------------------------------------------------------------------------
// File: CreditProcess.cpp
//
// Edit: 2021/03/22 野筒隼輔
//-----------------------------------------------------------------------------
#include "CreditProcess.h"
#include "Application/Game/Scene/Scene.h"

//-----------------------------------------------------------------------------
// Name: CreditProcess()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
CreditProcess::CreditProcess()
	: m_spBackTexture(nullptr)
	, m_spKeyTipsTexture(nullptr)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void CreditProcess::Deserialize(const json11::Json& json_object)
{
	GameObject::Deserialize(json_object);

	// テクスチャ読み込み
	m_spBackTexture		= KdResFac.GetTexture("Data/Texture/Credit/Back.png");
	m_spKeyTipsTexture	= KdResFac.GetTexture("Data/Texture/Credit/KeyTips.png");
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void CreditProcess::Update()
{
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

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void CreditProcess::DrawSprite()
{
	// 背景
	if (m_spBackTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spBackTexture.get(), 0, 0);
	}

	// キーのヒント
	if (m_spKeyTipsTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spKeyTipsTexture.get(), 562, -330);
	}
}