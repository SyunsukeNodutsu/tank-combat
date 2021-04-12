//-----------------------------------------------------------------------------
// File: InterruptCheck.cpp
//
// Edit: 2021/03/21 野筒隼輔
//-----------------------------------------------------------------------------
#include "InterruptCheck.h"

//-----------------------------------------------------------------------------
// Name: InterruptCheck()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
InterruptCheck::InterruptCheck()
	: m_buttonArray()
	, m_checkExitTexture(nullptr)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void InterruptCheck::Deserialize()
{
	// テクスチャ読み込み
	m_checkExitTexture = KdResFac.GetTexture("Data/Texture/System/CheckExit.png");

	//--------------------------------------------------
	// ボタン初期設定
	//--------------------------------------------------
	std::array<std::string, 3> path = {};
	path.fill("");
	constexpr Math::Vector2 size = Math::Vector2(110.0f, 40.0f);

	path = {
		"Data/Texture/System/CheckYES.png",
		"Data/Texture/System/CheckYES.png",
		"Data/Texture/System/CheckYES.png"
	};
	m_buttonArray[0].SetInfo(path, Math::Vector2(-100.0f, 0.0f), size);

	path = {
		"Data/Texture/System/CheckNO.png",
		"Data/Texture/System/CheckNO.png",
		"Data/Texture/System/CheckNO.png"
	};
	m_buttonArray[1].SetInfo(path, Math::Vector2(100.0f, 0.0f), size);
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void InterruptCheck::Update()
{
	if (!m_isEnable) { return; }

	// マウス情報所得
	POINT mouse_pos = {};

	// ボタン
	for (auto& button : m_buttonArray) {
		button.Update(mouse_pos);
	}
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void InterruptCheck::DrawSprite()
{
	if (!m_isEnable) { return; }

	// 確認の枠
	if (m_checkExitTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_checkExitTexture.get(), 0, 0);
	}

	// ボタン
	for (auto& button : m_buttonArray) {
		button.DrawSprite();
	}
}