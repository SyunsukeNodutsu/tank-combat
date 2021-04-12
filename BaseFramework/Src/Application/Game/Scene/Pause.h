//-----------------------------------------------------------------------------
// File: Pause.h
//
// Edit: 2021/03/18 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class Pause
// Desc: ポーズ(一時停止)管理クラス
//-----------------------------------------------------------------------------
class Pause
{
	class StateBase;
public:

	// 初期化
	void Deserialize();
	// 更新
	void Update();
	// 描画
	void DrawSprite();

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// インスタンスの所得
	static Pause& GetInstance() { static Pause instance; return instance; }
	// 有効か確認
	const bool IsEnable() const { return m_isEnable; }
	// ゲーム中かどうか確認
	const bool IsPlaying() const { return m_isPlaying; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	void SetGamePlaying(const bool playing) { m_isPlaying = playing; }

private:
	std::shared_ptr<StateBase>	m_spState;			// 現在の状態
	std::shared_ptr<KdTexture>	m_spBackTexture;	// 背景のテクスチャ
	std::shared_ptr<KdTexture>	m_spMissionTexture;	// 目的のテクスチャ
	std::shared_ptr<KdTexture>	m_spTipsTexture;	// 説明のテクスチャ
	float						m_tmpVolume;		// 一時的に音量を保存
	bool						m_isEnable;			// 有効かどうか(一時停止中かどうか)
	bool						m_isPlaying;		// ゲーム中どうか

private:
	Pause();
	void CheckChangeEnable();
	void LoadTexture();

private:
	//--------------------------------------------------
	// 状態ごとにステート化
	//--------------------------------------------------

	// 抽象ステートベース
	class StateBase
	{
	public:
		// 初期化
		virtual void Deserialize(Pause& owner) = 0;
		// 更新
		virtual void Update(Pause& owner, const POINT mouse_pos) = 0;
		// 2D描画
		virtual void DrawSprite(Pause& owner) = 0;
	};

	// コマンド選択待機状態
	class StateCommand : public StateBase
	{
		class StateBase;
	public:
		// コンストラクタ
		StateCommand();
		// 初期化
		void Deserialize(Pause& owner) override;
		// 更新
		void Update(Pause& owner, const POINT mouse_pos) override;
		// 2D描画
		void DrawSprite(Pause& owner) override;

	private:
		std::shared_ptr<StateBase>	m_spState;		// 状態(階層ステートマシン)
		std::array<SpriteButton, 2>	m_buttonArray;	// ボタン
		bool						m_stateOrder;	// これ無理やりな実装

	private:

		// コマンド状態管理 抽象インターフェース
		class StateBase
		{
		public:
			virtual void Initialize(Pause& owner) = 0;
			virtual void Update(Pause& owner, const POINT& mouse_pos) = 0;
			virtual void DrawSprite(Pause& owner) = 0;
		};

		// 命令ボタンを選択
		class StateOrder : public StateBase
		{
		public:
			StateOrder();
			void Initialize(Pause& owner) override;
			void Update(Pause& owner, const POINT& mouse_pos) override;
			void DrawSprite(Pause& owner) override;
		private:
			std::array<SpriteButton, 2>	m_buttonArray;	// ボタン
			int							m_frameCount;	// フレームのカウンタ
		};

		// システムボタンを選択
		class StateSystem : public StateBase
		{
		public:
			StateSystem();
			void Initialize(Pause& owner) override;
			void Update(Pause& owner, const POINT& mouse_pos) override;
			void DrawSprite(Pause& owner) override;
		private:
			std::array<SpriteButton, 3>	m_buttonArray;	// ボタン
			int							m_frameCount;	// フレームのカウンタ
		};
	};
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define PAUSE Pause::GetInstance()