//-----------------------------------------------------------------------------
// File: TutorialProcess.h
//
// Edit: 2021/04/08 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "GameProcess.h"

//-----------------------------------------------------------------------------
// Name: class TutorialProcess
// Desc: チュートリアルの管理
//-----------------------------------------------------------------------------
class TutorialProcess : public GameProcess
{
	class StateBase;
public:

	// コンストラクタ
	TutorialProcess();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;
	// 2D描画
	void DrawSprite() override;

private:
	std::shared_ptr<StateBase> m_spState; // 現在の状態

private:
	//==================================================
	// 
	// 状態管理
	// 
	//==================================================

	//--------------------------------------------------
	// Name: class StateBase
	// Desc: 抽象ステート
	//--------------------------------------------------
	class StateBase
	{
	public:
		virtual void Initialize(TutorialProcess& owner) = 0;
		virtual void Update(TutorialProcess& owner) = 0;
		virtual void DrawSprite(TutorialProcess& owner) = 0;
	};

	//--------------------------------------------------
	// Name: class State01
	// Desc: チュートリアル01 ... 基本操作.敵の撃破
	//--------------------------------------------------
	class State01 : public StateBase
	{
		enum class Progress;
	public:
		State01();
		virtual void Initialize(TutorialProcess& owner) override;
		virtual void Update(TutorialProcess& owner)  override;
		virtual void DrawSprite(TutorialProcess& owner)  override;

	private:
		Progress									m_progress;			// 進行具合
		std::array<std::shared_ptr<KdTexture>, 7>	m_spTextureArray;	// テクスチャ

		// 進行具合
		enum class Progress
		{
			eStart,	// 開始(すぐに始まってボタンを押してしまわないように)
			e01,	// 各機能の説明(右下の車体の2D描画と右下の弾のリロード)
			e02,	// カメラ操作の説明(マウスを操作)
			e03,	// 移動の説明(指定座標まで前進)
			e04,	// 射撃の説明(敵を倒させる)
			eEnd,	// 終了
		};
	};

	//--------------------------------------------------
	// Name: class State02
	// Desc: チュートリアル02 ... 味方へ命令.航空支援
	//--------------------------------------------------
	class State02 : public StateBase
	{
	public:
		virtual void Initialize(TutorialProcess& owner)  override;
		virtual void Update(TutorialProcess& owner)  override;
		virtual void DrawSprite(TutorialProcess& owner)  override;
	};
};