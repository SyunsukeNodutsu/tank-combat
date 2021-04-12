//-----------------------------------------------------------------------------
// File: TitleAnimationProcess.h
//
// Edit: 2021/03/29 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "GameProcess.h"

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
enum class ANIMATION_STATE;
class FixedCamera;

//-----------------------------------------------------------------------------
// Name: class TitleAnimationProcess
// Desc: タイトルのアニメーション管理
//-----------------------------------------------------------------------------
class TitleAnimationProcess : public GameProcess
{
public:

	// コンストラクタ
	TitleAnimationProcess();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;
	// 2D描画
	void DrawSprite() override;

private:
	ANIMATION_STATE					m_state;				// アニメーションの状態
	std::shared_ptr<FixedCamera>	m_spFixedCamera;		// 固定カメラ
	std::shared_ptr<KdTexture>		m_spLogoTexture;		// タイトルロゴ
	std::shared_ptr<KdTexture>		m_spFillBlackTexture;	// 遷移時の暗転
	std::shared_ptr<KdTexture>		m_spCinemaTexture;		// 映画のような視覚効果
	Math::Color						m_whiteBack;			// ブラックアウト時の色(RGBA)
	int								m_frameCnt;				// フレームのカウント

private:
	void LoadTexture();
	void CheckSkipProcess();
	void CheckChangeAnimation();
	void UpdateCamera();
	void UpdateBlackout();
};

//--------------------------------------------------
// Name: enum class ANIMATION_STATE
// Desc: アニメーションの状態管理
//--------------------------------------------------
enum class ANIMATION_STATE
{
	eScene01,
	eScene02,
	eScene03,
};