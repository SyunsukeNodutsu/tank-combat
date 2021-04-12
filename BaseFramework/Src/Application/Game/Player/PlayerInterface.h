//-----------------------------------------------------------------------------
// File: PlayerInterface.h
//
// Edit: 2021/04/09 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
class GameObject;
class PlayerTank;

//-----------------------------------------------------------------------------
// Name: class PlayerInterface
// Desc: 操作戦車のインターフェース 2D描画を担当 ※修正.テクスチャの管理がgm
//-----------------------------------------------------------------------------
class PlayerInterface
{
	enum class ReloadState;
public:

	// コンストラクタ
	PlayerInterface(PlayerTank& owner);
	// 初期化
	void Deserialize();
	// 更新
	void Update();
	// 2D描画
	void DrawSprite();

private:
	PlayerTank&					m_owner;				// オーナー
	ReloadState					m_reloadState;			// リロードの状態
	KdVector3					m_bulletTexturePos;		// 弾の装填アニメーション時の座標

	//--------------------------------------------------
	// テクスチャ ※修正.管理方法と命名がgm
	//--------------------------------------------------
	std::shared_ptr<KdTexture>	m_spThirdAimTex;		// 3人称時の照準器
	std::shared_ptr<KdTexture>	m_spFirstAim01Tex;		// 1人称時の照準器.周りの黒い丸
	std::shared_ptr<KdTexture>	m_spFirstAim02Tex;		// 1人称時の照準器.周りの黒い点々
	std::shared_ptr<KdTexture>	m_spFirstAim03Tex;		// 1人称時の照準器.三角形のやつ

	std::shared_ptr<KdTexture>	m_spBodyTex;			// 角度を表現.車体
	std::shared_ptr<KdTexture>	m_spTurretTex;			// 角度を表現.砲塔
	std::shared_ptr<KdTexture>	m_spApBulletTex;		// 戦車の弾.AP弾
	std::shared_ptr<KdTexture>	m_spFirstVisibilityTex;	// 1人称時の視界
	std::shared_ptr<KdTexture>	m_spThirdVisibilityTex;	// 3人称時の視界

	std::shared_ptr<KdTexture>	m_spBlackRect01Tex;		// 黒の四角形.左下 ※大きいほう
	std::shared_ptr<KdTexture>	m_spBlackRect02Tex;		// 黒の四角形.右下 ※小さいほう
	std::shared_ptr<KdTexture>	m_spFillRect01Tex;		// 塗りつぶした四角形.左下 ※大きいほう
	std::shared_ptr<KdTexture>	m_spFillRect02Tex;		// 塗りつぶした四角形.右下 ※小さいほう
	std::shared_ptr<KdTexture>	m_spFillRect03Tex;		// リロード完了を知らせる

	std::shared_ptr<KdTexture>	m_spRedTriangleTex;		// 頭上のマーク.赤(敵)
	std::shared_ptr<KdTexture>	m_spGreenTriangleTex;	// 頭上のマーク.緑(味方)

	// 数字のテクスチャ配列
	std::array<std::shared_ptr<KdTexture>, 10> m_spNumberTexArray;

	// リロード状態
	enum class ReloadState
	{
		eStart,	// 射撃した直後
		eCenter,// リロードの中間
		eEnd,	// リロード完了
	};

private:
	void LoadTexture();
	void UpdateReload();
	void DrawSpriteStateMove();
	void DrawSpriteAimMove();
	void DrawSpriteOverheadMark(int screenH, int screenW, KdMatrix viewportMat, KdMatrix viewMat, KdMatrix projMat, const std::shared_ptr<GameObject> object);
};