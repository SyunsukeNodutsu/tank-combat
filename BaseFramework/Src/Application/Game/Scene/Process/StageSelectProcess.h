//-----------------------------------------------------------------------------
// File: StageSelectProcess.h
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: class StageSelectProcess
// Desc: ステージ選択の管理
//-----------------------------------------------------------------------------
class StageSelectProcess : public GameObject
{
	enum class SelectStage;
public:

	// コンストラクタ
	StageSelectProcess();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;
	// 2D描画
	void DrawSprite() override;

private:
	SelectStage					m_selectStage;		// ボタンの選択状況
	SelectStage					m_prevSelectStage;	// 1フレーム前のボタンの選択状況
	std::array<SpriteButton, 4> m_buttonArray;		// ボタン
	std::shared_ptr<KdTexture>	m_spBackTexture;	// 背景テクスチャ
	std::shared_ptr<KdTexture>	m_spKeyTipsTexture;	// 入力のヒントテクスチャ
	std::shared_ptr<KdTexture>	m_spLogoTexture;	// ロゴテクスチャ
	int							m_frameCount;		// フレームのカウント ※ボタンの出現を管理

	Easing						m_easingMapHeight;	// MAPのEasing(縦)
	Easing						m_easingMapWidth;	// MAPのEasing(横)

	// ボタンの種類
	enum class SelectStage
	{
		eNone,// 選択していない
		e01, e02, e03, e04,// 各ステージ ※1と2はチュートリアル
	};

private:
	void LoadTexture();
	void SettingButton();
	void UpdateButton(const POINT mouse_pos);
	const KdVector3 CheckBackTexPos(const SelectStage select);
};