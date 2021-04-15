//-----------------------------------------------------------------------------
// File: Tank.h
//
// Edit: 2021/03/17 野筒隼輔
//-----------------------------------------------------------------------------
#include "Tank.h"
#include "TankParts.h"
#include "CannonBall.h"

#include "Application/Component/CameraComponent.h"
#include "Application/Component/InputComponent.h"

#include "Application/Manager/CameraManager.h"
#include "Application/Manager/ObjectManager.h"

//-----------------------------------------------------------------------------
// Name: Tank()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
Tank::Tank()
	: m_spTankParts(nullptr)
	, m_position(KdVector3(0.0f, 0.0f, 0.0f))
	, m_prevPosition(KdVector3(0.0f, 0.0f, 0.0f))
	, m_force(KdVector3(0.0f, 0.0f, 0.0f))
	, m_movePow(0.0f)
	, m_rotatePow(0.0f)
	, m_reloadCount(150)
	, m_reloadCountMax(150)
	, m_isDiscovery(false)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void Tank::Deserialize(const json11::Json& json_object)
{
	// 基底クラスの初期化(共通の初期化)
	GameObject::Deserialize(json_object);
	// 初期座標を設定
	m_position = m_worldMatrix.GetTranslation();
	// コンポーネント設定
	ComponentInitialSetting();
	// リソース設定
	ResourceInitialSetting();

	// 部品生成.初期化
	m_spTankParts = std::make_shared<TankParts>(*this);
	if (m_spTankParts) { m_spTankParts->Deserialize(json_object); }
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void Tank::Update()
{
	if (CAMERA_MAGER.IsAnimation()) { return; }

	// コントローラ更新
	if (m_spInputComponent) { m_spInputComponent->Update(); }

	// 移動前の座標記憶
	m_prevPosition = m_position;

	// 状態ごとの更新 ※派生先で処理
	UpdateByState();

	// 座標確定
	m_worldMatrix.SetTranslation(m_position);

	// 部品更新
	if (m_spTankParts) { m_spTankParts->Update(); }

	// リロードのカウント減少
	m_reloadCount--;

	// 追従カメラに座標を設定
	if (m_spCameraComponent) {
		KdMatrix transMatrix = {};
		transMatrix.CreateTranslation(m_worldMatrix.GetTranslation());
		m_spCameraComponent->SetCameraMatrix(transMatrix);
	}
}

//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: 描画
//-----------------------------------------------------------------------------
void Tank::Draw()
{
	GameObject::Draw();
	if (m_spTankParts) { m_spTankParts->Draw(); }
}

//-----------------------------------------------------------------------------
// Name: DrawShadowMap()
// Desc: シャドウマップ描画
//-----------------------------------------------------------------------------
void Tank::DrawShadowMap()
{
	GameObject::DrawShadowMap();
	if (m_spTankParts) { m_spTankParts->DrawShadowMap(); }
}

//-----------------------------------------------------------------------------
// Name: NoticeAerialBombLanding()
// Desc: 航空機の爆弾の着弾通知
//-----------------------------------------------------------------------------
void Tank::NoticeAerialBombLanding(const KdVector3 position)
{
	// 爆撃の影響を受ける範囲
	const float damageRange = 25.0f;

	// 衝突確認(四角形の中にあるか)
	bool isHit = (
		position.x - damageRange < m_position.x&& position.x + damageRange > m_position.x &&
		position.z - damageRange < m_position.z&& position.z + damageRange > m_position.z
		);

	if (isHit == false) { return; }
	// 死亡
	Destroy();
}

//-----------------------------------------------------------------------------
// Name: UpdateMove()
// Desc: 移動更新 ※修正.マジックナンバー
//-----------------------------------------------------------------------------
void Tank::UpdateMove(const float input_axisY)
{
	//--------------------------------------------------
	// 加減速
	//--------------------------------------------------
	
	// 前進
	if (input_axisY > 0) {
		if (m_movePow <= 0.18f) { m_movePow += 0.002f; }
	}
	// 後退
	else if (input_axisY < 0) {
		if (m_movePow >= -0.18f) { m_movePow -= 0.002f; }
	}
	// 入力なし
	else {
		// 減速
		if (m_movePow > 0) { m_movePow -= 0.006f; }
		else if (m_movePow < 0) { m_movePow += 0.006f; }

		// 停止 ※0.02fの範囲は0とする
		if (m_movePow >= -0.02f && m_movePow <= 0.02f) { m_movePow = 0.0f; }
	}

	//--------------------------------------------------
	// 移動行列作成.合成
	//--------------------------------------------------

	KdMatrix transMat = {};
	transMat.CreateTranslation(0.0f, 0.0f, m_movePow);
	m_worldMatrix = transMat * m_worldMatrix;

	// 移動量 ... 現在の座標から1フレーム前の座標を減算 ※修正.ややこしい
	m_force.x = m_worldMatrix.GetTranslation().x - m_prevPosition.x;
	m_force.z = m_worldMatrix.GetTranslation().z - m_prevPosition.z;

	// 重力による落下
	constexpr float gravity = 0.006f;
	m_force.y -= gravity;

	// 座標確定
	m_position += m_force;
}

//-----------------------------------------------------------------------------
// Name: UpdateRotateBody()
// Desc: 車体の回転更新
//-----------------------------------------------------------------------------
void Tank::UpdateRotateBody(const float input_axisX)
{
	//--------------------------------------------------
	// 加減速
	//--------------------------------------------------
	
	// 右旋回
	if (input_axisX > 0) {
		if (m_rotatePow <= 0.4f) { m_rotatePow += 0.04f; }
	}
	// 左旋回
	else if (input_axisX < 0) {
		if (m_rotatePow >= -0.4f) { m_rotatePow -= 0.04f; }
	}
	// 入力なし
	else {
		// 減速
		if (m_rotatePow > 0) { m_rotatePow -= 0.02f; }
		else if (m_rotatePow < 0) { m_rotatePow += 0.02f; }

		// 停止 ※0.02fの範囲は0とする
		if (m_rotatePow >= -0.02f && m_rotatePow <= 0.02f) { m_rotatePow = 0.0f; }
	}

	//--------------------------------------------------
	// 回転行列作成.合成
	//--------------------------------------------------
	KdMatrix rotateMat = {};
	rotateMat.CreateRotationX(0);
	rotateMat.RotateY(m_rotatePow * KdToRadians);
	m_worldMatrix = rotateMat * m_worldMatrix;
}

//-----------------------------------------------------------------------------
// Name: UpdateRunEffect()
// Desc: 移動時のエフェクト更新
//-----------------------------------------------------------------------------
void Tank::UpdateRunEffect(const Math::Vector2 input_move)
{
	if (input_move.LengthSquared() == 0) { return; }
	if (m_runEffect == nullptr) { return; }

	// Effect再生
	m_runEffect->Play(m_worldMatrix.GetTranslation());
	EFFEKSEER.GetManager()->SetMatrix(m_runEffect->GetHandle(), m_worldMatrix);

	// Effectを車体の後ろに移動
	const float rotate_bodyY = atan2f(m_worldMatrix.GetAxisZ().x, m_worldMatrix.GetAxisZ().z) * KdToDegrees * -1;
	KdVector3 position = KdVector3::GetLaunchPoint(-90.0f + (rotate_bodyY), 0.0f);
	position.y -= 1.0f;
	m_runEffect->Move(position);
}

//-----------------------------------------------------------------------------
// Name: UpdateCollision()
// Desc: 衝突判定更新
//-----------------------------------------------------------------------------
void Tank::UpdateCollision()
{
	// 許容する段差
	constexpr float allowToStepHeight = 0.6f;

	//--------------------------------------------------
	// 横方向との判定
	//--------------------------------------------------

	// ぶつかり判定
	CheckBump();

	//--------------------------------------------------
	// 下方向との判定
	//--------------------------------------------------

	// レイの発射地点 ※Y座標を少し浮かし.レイの発射場所が埋まる問題を解決
	std::array<KdVector3, 4> ray_pos = {};
	ray_pos.fill(m_position + KdVector3(0.0f, 0.8f, 0.0f));
	const float rotate_bodyY = atan2f(m_worldMatrix.GetAxisZ().x, m_worldMatrix.GetAxisZ().z) * KdToDegrees * -1;
	ray_pos[0] += KdVector3::GetLaunchPoint(  60.0f + (rotate_bodyY), 2.5f);	// 右上 ※戦車のZ軸が90度なので右横が0度
	ray_pos[1] += KdVector3::GetLaunchPoint( 120.0f + (rotate_bodyY), 2.5f);	// 左上
	ray_pos[2] += KdVector3::GetLaunchPoint( -60.0f + (rotate_bodyY), 2.5f);	// 右下
	ray_pos[3] += KdVector3::GetLaunchPoint(-120.0f + (rotate_bodyY), 2.5f);	// 左下

	// 判定 ※発射地点は車体の真ん中
	float distanceFromGround = FLT_MAX;
	KdRayResult result = CheckGround(distanceFromGround, m_position);
	if (result.m_hit) {
		m_position.y += allowToStepHeight - distanceFromGround;
		m_force.y = 0.0f;
	}

	//--------------------------------------------------
	// 車体の傾き
	//--------------------------------------------------
	TiltBody(ray_pos, distanceFromGround);
}

//-----------------------------------------------------------------------------
// Name: UpdateShot()
// Desc: 射撃更新
//-----------------------------------------------------------------------------
void Tank::UpdateShot(const KdVector3 enemy_ver_axiz)
{
	if (m_reloadCount > 0) { return; }
	if (m_spTankParts == nullptr) { return; }

	// 主砲情報所得
	auto main_gun = m_spTankParts->GetMainGun();
	if (main_gun == nullptr) {
		assert(0 && "[UpdateShot] : 主砲が存在しません");
		return;
	}

	// リロード時間リセット
	m_reloadCount = m_reloadCountMax;
	// 攻撃回数加算
	m_AttackNum++;

	//--------------------------------------------------
	// 砲弾作成.発射
	//--------------------------------------------------

	// 砲弾の発射軸
	KdVector3 emit_axis = main_gun->GetMatrix().GetAxisZ();
	// 砲弾が出てくる座標(主砲の先端)
	KdVector3 emit_pos = main_gun->GetMatrix().GetTranslation();
	emit_pos += (emit_axis * 4.0f);// 少し伸ばす

	// 砲弾作成
	std::shared_ptr<CannonBall> cannon_ball = std::make_shared<CannonBall>(*this);
	if (cannon_ball) {
		// 初期化.設定
		cannon_ball->Deserialize(KdResFac.GetJson("Data/Text/Object/Weapon/CannonBall.json"));
		cannon_ball->Fire(emit_pos, emit_axis);
		OBJ_MAGER.AddObject(cannon_ball);
	}

	//--------------------------------------------------
	// 演出効果
	//--------------------------------------------------
	
	// エフェクト
	if (m_fireEffect) { m_fireEffect->Play(emit_pos); }

	// 効果音
	if (m_spFireSE) {
		m_spFireSE->Play(false);
		m_spFireSE->SetVolume(2.0f);
		m_spFireSE->SetPosition(emit_pos);
	}
}

//-----------------------------------------------------------------------------
// Name: UpdateRunBGM()
// Desc: 移動時のキャタピラの音更新 ※修正.マジックナンバー
//-----------------------------------------------------------------------------
void Tank::UpdateRunBGM()
{
	if (m_spRunBGM == nullptr) { return; }

	// 移動.回転値をもとに音量を設定
	const float move_pow = GetMoveSpeed();
	const float rotate_pow = GetRotateSpeed();

	// 移動量(入力値)を元に音量設定
	constexpr float min_volume = 0.02f;// エンジンは動いているのでこの値は聞こえる
	float volume = min_volume + (fabsf(m_movePow) * 0.8f) + (fabsf(rotate_pow) * 0.2f);

	// 最大音量
	constexpr float max_volume = 0.2f;
	volume = std::clamp(volume, -max_volume, max_volume);

	// 座標.音量の設定
	m_spRunBGM->SetPosition(m_position);
	m_spRunBGM->SetVolume(volume);
}

//-----------------------------------------------------------------------------
// Name: ComponentInitialSetting()
// Desc: コンポーネントの初期設定
//-----------------------------------------------------------------------------
void Tank::ComponentInitialSetting()
{
	// カメラ作成.初期化 ※敵は撃破時のアニメーションに使用
	m_spCameraComponent = std::make_shared<CameraComponent>(*this);
	if (m_spCameraComponent) {
		m_spCameraComponent->OffsetMatrix().Move(0.0f, 3.0f, -8.0f);
	}
}

//-----------------------------------------------------------------------------
// Name: ResourceInitialSetting()
// Desc: リソース初期設定
//-----------------------------------------------------------------------------
void Tank::ResourceInitialSetting()
{
	// エフェクト読み込み
	m_runEffect		= EFFEKSEER.LoadEffect(u"Data/Effect/TankRun.efk");
	m_deadSmkEffect = EFFEKSEER.LoadEffect(u"Data/Effect/Smoke.efk");
	m_deadExpEffect = EFFEKSEER.LoadEffect(u"Data/Effect/Explosion.efk");
	m_fireEffect	= EFFEKSEER.LoadEffect(u"Data/Effect/TankFire.efk");

	// 効果音読み込み
	m_spFireSE = AUDIO.GetSoundInstance3D("Data/Audio/SE/TankFire.wav", m_position);

	// BGM読み込み.再生
	m_spRunBGM = AUDIO.GetSoundInstance3D("Data/Audio/BGM/TankRun.wav", m_position);
	if (m_spRunBGM) {
		m_spRunBGM->SetVolume(0.0f);
		m_spRunBGM->Play(true);
	}
}

//-----------------------------------------------------------------------------
// Name: CheckGround()
// Desc: 地面との判定
//-----------------------------------------------------------------------------
KdRayResult Tank::CheckGround(float& dst_distance, KdVector3 ray_pos)
{
	// 許容する段差
	constexpr float allowToStepHeight = 0.6f;

	// レイ情報設定
	RayInfo rayInfo = {};
	rayInfo.m_pos = ray_pos;
	rayInfo.m_pos.y += allowToStepHeight;				// 段差を考慮
	rayInfo.m_pos.y += m_prevPosition.y - m_position.y;	// 移動値を考慮
	rayInfo.m_dir = KdVector3(0.0f, -1.0f, 0.0f);
	rayInfo.m_maxRange = FLT_MAX;

	// 全Objectと判定
	KdRayResult finalRayResult = {};
	for (auto& object : OBJ_MAGER.GetObjectList()) {
		// 地面が対象
		if (object.get() == this) { continue; }
		if (object->GetTag() & OBJECT_TAG::TAG_StageObject) {
			if (object->GetCollisionComponent() == nullptr) { continue; }
			// 判定
			KdRayResult rayResult = {};
			if (!object->GetCollisionComponent()->HitCheckByRay(rayInfo, rayResult)) { continue; }
			// 最短距離を確認
			if (rayResult.m_distance < finalRayResult.m_distance) { finalRayResult = rayResult; }
		}
	}

	// 当たった
	float distanceFromGround = FLT_MAX;
	if (finalRayResult.m_hit) {
		// 地面との距離を算出(補正値も考慮)
		distanceFromGround = finalRayResult.m_distance - (m_prevPosition.y - m_position.y);
	}

	// 地面との距離を格納
	dst_distance = distanceFromGround;

	return finalRayResult;
}

//-----------------------------------------------------------------------------
// Name: CheckBump()
// Desc: ぶつかり判定
//-----------------------------------------------------------------------------
void Tank::CheckBump()
{
	//--------------------------------------------------
	// 情報設定
	//--------------------------------------------------

	// 車体の前に配置する球情報
	SphereInfo frontSphereInfo = {};
	frontSphereInfo.m_pos = m_position + KdVector3::GetLaunchPoint(-90.0f, 2.0f);
	frontSphereInfo.m_pos.y += 1.0f;
	frontSphereInfo.m_radius = 1.4f;

	// 車体の後ろに配置する球情報
	SphereInfo backSphereInfo = {};
	backSphereInfo.m_pos = m_position + KdVector3::GetLaunchPoint(90.0f, 1.8f);
	backSphereInfo.m_pos.y += 1.0f;
	backSphereInfo.m_radius = 1.4f;

	// 結果格納用
	SphereResult frontSphereResult = {};
	SphereResult backSphereResult = {};

	//--------------------------------------------------
	// 判定
	//--------------------------------------------------
	for (auto& object : OBJ_MAGER.GetObjectList()) {
		if (object.get() == this) { continue; }
		if (object->GetTag() == OBJECT_TAG::TAG_StageObject) { continue; }
		// 家
		if (object->GetTag() == OBJECT_TAG::TAG_HouseObject) {
			// 前
			if (object->GetCollisionComponent()->HitCheckBySphereVsMesh(frontSphereInfo, frontSphereResult)) {
				m_position += frontSphereResult.m_push;
			}
			// 後ろ
			if (object->GetCollisionComponent()->HitCheckBySphereVsMesh(backSphereInfo, backSphereResult)) {
				m_position += backSphereResult.m_push;
			}
		}
		// 木
		if (object->GetTag() & OBJECT_TAG::TAG_TreeObject) {

		}
		// 草の壁
		if (object->GetTag() & OBJECT_TAG::TAG_GrassObject) {

		}
	}
}

//-----------------------------------------------------------------------------
// Name: TiltBody()
// Desc: 車体の傾き
//-----------------------------------------------------------------------------
void Tank::TiltBody(std::array<KdVector3, 4>& ray_pos, float& dst_distance)
{
	//--------------------------------------------------
	// 法線を算出
	//--------------------------------------------------

	// 4点の判定
	for (auto& raypos : ray_pos) {
		KdRayResult result = CheckGround(dst_distance, raypos);
		if (result.m_hit) { raypos = result.m_hitPos; }
	}

	// 4端をつなぐ線
	KdVector3 upLine	= ray_pos[1] - ray_pos[0];
	KdVector3 leftLine	= ray_pos[1] - ray_pos[3];
	KdVector3 downLine	= ray_pos[2] - ray_pos[0];
	KdVector3 rightLine = ray_pos[2] - ray_pos[3];

	// 外積算出
	KdVector3 cross = KdVector3::Cross(upLine, leftLine) + KdVector3::Cross(rightLine, downLine);
	cross.Normalize();

	//--------------------------------------------------
	// なす角分回転させる
	//--------------------------------------------------

	// 車体のY軸
	KdVector3 body_axizY = m_worldMatrix.GetAxisY();
	body_axizY.Normalize();

	// この軸で回転させる
	KdVector3 rot_axis = KdVector3::Cross(body_axizY, cross);
	if (rot_axis.LengthSquared() == 0) { return; }

	// 内積を算出
	float dot = KdVector3::Dot(body_axizY, cross);
	constexpr float dot_max = 1.0f;
	dot = std::clamp(dot, -dot_max, dot_max);// 範囲内に補正

	// なす角算出
	float radian = acos(dot);

	// 1.0fの角度制限を設定 ※急にガクッてなるのを防ぐ
	constexpr float rotate_max = 1.0f * KdToRadians;
	if (radian > rotate_max) { radian = rotate_max; }

	// 指定の軸(Y)でなす角分回転させる
	KdMatrix rotMatrix = {};
	rotMatrix.CreateRotationAxis(rot_axis, radian);
	m_worldMatrix *= rotMatrix;
}