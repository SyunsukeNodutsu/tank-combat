//-----------------------------------------------------------------------------
// File: Easing.h
//
// Edit: 2021/03/08 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
enum class EASING_TYPE;

//-----------------------------------------------------------------------------
// Name: class Easing
// Desc: イージングクラス ※1つの値のみに対応.目標をX座標として考える
//-----------------------------------------------------------------------------
class Easing
{
public:

	// コンストラクタ
	Easing();

	// 更新 ※注意.必ずEmitの後に記述
	void Update();

	// 発生
	void Emit(EASING_TYPE a_type, float a_timeLength, float a_startX, float a_endX);

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// 進行具合所得
	float GetMovement() { return m_progress; }
	// 完了しているか
	bool IsEmit() { return m_isEmit; }

private:
	EASING_TYPE		m_easingType;	// イージングの種類
	CommonTimer		m_timer;		// 時間計測用タイマー
	bool			m_isEmit;		// 発生しているか
	float			m_startX;		// 移動開始座標
	float			m_endX;			// 移動終了座標
	float			m_easingTime;	// アニメーションさせる時間
	float			m_progress;		// 移動量
	float			m_nowTime;		// 現在の時間
};

//--------------------------------------------------
// Name: enum class EASING_TYPE
// Desc: イージングの種類
//--------------------------------------------------
enum class EASING_TYPE
{
	eLinear,
	eConstant,
	eQuadIn,
	eQuadOut,
	eQuadInOut,
	eCubicIn,
	eCubicOut,
	eCubicInOut,
	eQuartIn,
	eQuartOut,
	eQuartInOut,
	eQuintIn,
	eQuintOut,
	eQuintInOut,
	eSineIn,
	eSineOut,
	eSineInOut,
	eExpIn,
	eExpOut,
	eExpInOut,
	eCircIn,
	eCircOut,
	eCircInOut,
	eElasticIn,
	eElasticOut,
	eElasticInOut,
	eBackIn,
	eBackOut,
	eBackInOut,
	eBounceIn,
	eBounceOut,
	eBounceInOut,
};