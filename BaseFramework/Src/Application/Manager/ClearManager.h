//-----------------------------------------------------------------------------
// File: ClearManager.h
//
// Edit: 2021/03/27 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
class Tank;

//-----------------------------------------------------------------------------
// Name: class ClearManager
// Desc: ゲームのクリアを管理 ※修正.データの受け渡しが雑、素直に外部ファイルを使ったほうがいい
//-----------------------------------------------------------------------------
class ClearManager
{
public:

	// 初期化
	void Deserialize(const json11::Json& json_object);
	// 更新
	void Update();
	// 2D描画
	void DrawSprite();

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// インスタンスの所得
	static ClearManager& GetInstance() { static ClearManager instance; return instance; }
	// ゲームが終了したか所得
	const bool IsGameEnd() const { return (m_isClear || m_isGameOver); }
	// ゲームをクリアしたかどうか所得
	const bool IsGameClear() const { return m_isClear; }
	// 味方戦車のコンテナを所得
	const std::list<std::shared_ptr<Tank>> GetFriendlyTankList() { return m_friendlyTankList; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// 敵の撃破通知を受け取る
	void EnemyDefeat();
	// 味方戦車のコンテナを設定
	void SetFriendlyTankList(const std::list<std::shared_ptr<Tank>> list) { m_friendlyTankList = list; }

private:
	std::list<std::shared_ptr<Tank>>	m_friendlyTankList;			// 味方戦車のコンテナ
	std::shared_ptr<KdTexture>			m_spClearStringTexture;		// テクスチャ.クリアの文字
	std::shared_ptr<KdTexture>			m_spGameOverStringTexture;	// テクスチャ.ゲームオーバーの文字
	std::uint8_t						m_enemyCount;				// ステージで倒すべき敵の数
	Math::Color							m_textureColor;				// テクスチャの色
	bool								m_isClear;					// クリアしたかどうか
	bool								m_isGameOver;				// ゲームオーバーかどうか

private:
	ClearManager();
	void LoadTexture();
	void SettingParameter(const json11::Json& json_object);
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define CLEAR_MAGER ClearManager::GetInstance()