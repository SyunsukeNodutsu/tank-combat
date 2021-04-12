//-----------------------------------------------------------------------------
// File: Radio.h
//
// Edit: 2021/03/27 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
enum class WIRELESS_TYPE;
enum class OWNER_TYPE;

//-----------------------------------------------------------------------------
// Name: class Radio
// Desc: 無線クラス 情報のやり取り
//-----------------------------------------------------------------------------
class Radio
{
public:

	// 初期化
	void Deserialize();
	// 更新
	void Update();
	// 2D描画
	void DrawSprite();

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// インスタンスの所得
	static Radio& GetInstance() { static Radio instance; return instance; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// 無線発生
	void Emit(WIRELESS_TYPE stringType, OWNER_TYPE ownerType, const bool sePlay = false, const int count = 60);

private:
	WIRELESS_TYPE					m_stringType;	// 無線の文字列の種類(内容)
	OWNER_TYPE						m_ownerType;	// 無線を出したオーナー
	int								m_enableCount;	// 無線を表示させる時間(フレーム)

	// テクスチャ
	std::shared_ptr<KdTexture>										m_spRectTexture;		// 無線の画面の矩形
	std::unordered_map<WIRELESS_TYPE, std::shared_ptr<KdTexture>>	m_spStringTextureMap;	// 無線の内容(文字列)コンテナ
	std::unordered_map<OWNER_TYPE, std::shared_ptr<KdTexture>>		m_spOwnerTextureMap;	// オーナー(誰が行った無線か)コンテナ

private:
	Radio();
	void LoadTexture();
};

//--------------------------------------------------
// Name: enum WIRELESS_TYPE
// Desc: 無線の内容.種類
//--------------------------------------------------
enum class WIRELESS_TYPE
{
	eStartAttack,	// 攻撃開始
	eDefeat,		// 敵を撃破
	eBeShot,		// 敵の攻撃を受けた
	eRepelled,		// 装甲にはじかれた
	eDiscovery,		// 敵を発見
	eReturn,		// ステージクリア
	eDeathAllies,	// 味方戦車が戦闘不能
	eFriendlyFire,	// 味方を攻撃
	eAdvanceOrder,	// 命令.前進
	eWaitOrder,		// 命令.待機
	eRecessionOrder,// 命令.後退
	eFollowOrder,	// 命令.支援
	eTypeMax,
};

//--------------------------------------------------
// Name: enum OWNER_TYPE
// Desc: 無線のオーナー
//--------------------------------------------------
enum class OWNER_TYPE
{
	ePlayer,	// 操作キャラ(戦車長)
	eAllies,	// 01号車(味方)
	eAllies02,	// 02号車(味方)
	eAllies03,	// 03号車(味方)
	eHQ,		// 指令本部(大隊本部)
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define RADIO Radio::GetInstance()