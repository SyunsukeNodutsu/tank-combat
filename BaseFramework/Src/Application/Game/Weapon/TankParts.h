//-----------------------------------------------------------------------------
// File: TankParts.h
//
// Edit: 2021/03/28 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
class GameObject;
class Tank;

//-----------------------------------------------------------------------------
// Name: class TankParts
// Desc: 戦車の部品クラス 主砲.砲塔.キャタピラ
//-----------------------------------------------------------------------------
class TankParts
{
public:

	// コンストラクタ
	TankParts(Tank& owner);
	// 初期化
	void Deserialize(const json11::Json& json_object);
	// 更新
	void Update();
	// 描画
	void Draw();
	// シャドウ描画
	void DrawShadowMap();

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// 主砲所得
	const std::shared_ptr<GameObject> GetMainGun() const { return m_mainGun; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// 砲塔を向けたい軸を設定
	void SetTargetAxis(const KdVector3 axis) { m_targetAxis = axis; }
	// 主砲.砲塔の回転ON.OFF
	void SetTurretRot(const bool isRot) { m_isRot = isRot; }
	// 死亡時に主砲と砲塔を上に傾ける
	void Crash();

private:
	// オーナー
	Tank& m_owner;

	// 主砲
	std::shared_ptr<GameObject>		m_mainGun;
	KdMatrix						m_mainGunLocalMat;
	float							m_mainGunRotNum;

	// 砲塔
	std::shared_ptr<GameObject>		m_turret;
	KdMatrix						m_turretLocalMat;
	KdVector3						m_targetAxis;
	float							m_turretRotNum;
	float							m_turretRotSpeed;

	// 主砲.砲塔を回転させるか
	bool							m_isRot;

	// 左のキャタピラ
	std::shared_ptr<GameObject>		m_leftTrack;
	KdMatrix						m_leftTrackLocalMat;

	// 右のキャタピラ
	std::shared_ptr<GameObject>		m_rightTrack;
	KdMatrix						m_rightTrackLocalMat;

private:
	const bool SettingMainGun(const json11::Json& json_object);
	const bool SettingTurret(const json11::Json& json_object);
	const bool SettingTrack(const json11::Json& json_object);
	void UpdateMainGun();
	void UpdateTurret();
	void UpdateTrack();
};