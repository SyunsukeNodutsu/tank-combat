//-----------------------------------------------------------------------------
// File: TitleProcess.h
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "GameProcess.h"

//-----------------------------------------------------------------------------
// Name: class TitleProcess
// Desc: タイトルの管理 ※UIを表示.選択プロセス
//-----------------------------------------------------------------------------
class TitleProcess : public GameProcess
{
public:

	// コンストラクタ
	TitleProcess();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;
	// 2D描画
	void DrawSprite() override;

private:
	std::array<SpriteButton, 4>		m_buttonArray;		// ボタン()
	std::shared_ptr<KdTexture>		m_spTitleLogoTex;	// テクスチャ.タイトルロゴ
	std::shared_ptr<KdTexture>		m_spKeyTipsTex;		// テクスチャ.キー入力のヒント
	KdVector3						m_viewPosition;		// 注視点 ※デバッグ用
	bool							m_isPushRButton;	// マウスの右ボタンを押したか
	int								m_frameCount;		// フレームのカウント ※ボタンの出現を管理

	// ボタンの種類
	enum TitleButton { eStart, eOption, eCredit, eExit, };

private:
	void ButtonSetting();
	void UpdateButton(const POINT mouse_pos);
	void UpdateCamera(const POINT mouse_pos);
};