//-----------------------------------------------------------------------------
// File: PlayerInterface.h
//
// Edit: 2021/04/09 野筒隼輔
//-----------------------------------------------------------------------------
#include "PlayerInterface.h"
#include "PlayerTank.h"

#include "Application/Game/Weapon/TankParts.h"
#include "Application/Component/CameraComponent.h"
#include "Application/Component/ModelComponent.h"
#include "Application/Manager/CameraManager.h"
#include "Application/Manager/ObjectManager.h"

//-----------------------------------------------------------------------------
// Name: PlayerInterface()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
PlayerInterface::PlayerInterface(PlayerTank& owner)
	: m_owner(owner)
	, m_reloadState(ReloadState::eStart)
	, m_bulletTexturePos(KdVector3(0.0f, 0.0f, 0.0f))
	, m_spThirdAimTex(nullptr)
	, m_spFirstAim01Tex(nullptr)
	, m_spFirstAim02Tex(nullptr)
	, m_spFirstAim03Tex(nullptr)
	, m_spBodyTex(nullptr)
	, m_spTurretTex(nullptr)
	, m_spApBulletTex(nullptr)
	, m_spFirstVisibilityTex(nullptr)
	, m_spThirdVisibilityTex(nullptr)
	, m_spBlackRect01Tex(nullptr)
	, m_spBlackRect02Tex(nullptr)
	, m_spFillRect01Tex(nullptr)
	, m_spFillRect02Tex(nullptr)
	, m_spFillRect03Tex(nullptr)
	, m_spRedTriangleTex(nullptr)
	, m_spGreenTriangleTex(nullptr)
	, m_spNumberTexArray()
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void PlayerInterface::Deserialize()
{
	// テクスチャ読み込み
	LoadTexture();
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void PlayerInterface::Update()
{
	// リロード更新
	UpdateReload();
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void PlayerInterface::DrawSprite()
{
	if (CAMERA_MAGER.IsAnimation()) { return; }

	// 状態に応じた2D描画
	if (!m_owner.IsAim()) {
		// 3人称の時
		DrawSpriteStateMove();
	}
	else {
		// 1人称の時
		DrawSpriteAimMove();
	}

	// 以下 1.3人称共通の2D描画

	//--------------------------------------------------
	// 装填の状態
	//--------------------------------------------------
	{
		// UI
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spFillRect02Tex.get(), 480, -300, nullptr, &kTranslucent);

		// 攻撃可能なら、点滅させる
		static int g_count = 0;
		g_count++;
		if (g_count > 30) { g_count = 0; }
		if (m_reloadState == ReloadState::eEnd && g_count > 15) {
			SHADER.m_spriteShader.DrawTex(m_spFillRect03Tex.get(), 480, -300, nullptr, &kTranslucent);
		}
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spBlackRect02Tex.get(), 480, -300);

		// 弾の装填
		KdMatrix matrix_01 = {};
		matrix_01.Move(m_bulletTexturePos);
		SHADER.m_spriteShader.SetMatrix(matrix_01);
		SHADER.m_spriteShader.DrawTex(m_spApBulletTex.get(), 0, 0);
	}

	//--------------------------------------------------
	// 戦車の状態
	//--------------------------------------------------
	{
		// UI
		if (m_spFillRect01Tex) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(m_spFillRect01Tex.get(), -540, -260, nullptr, &kTranslucent);
		}

		// 車体のY軸回転
		const float body_rotateY = atan2f(m_owner.GetMatrix().GetAxisZ().x, m_owner.GetMatrix().GetAxisZ().z);

		// カメラのY軸回転
		KdVector3 camera_axisZ = m_owner.GetCameraComponent()->GetCameraMatrix().GetAxisZ();
		float camera_rotateY = atan2f(camera_axisZ.x, camera_axisZ.z);

		// 車体
		if (m_spBodyTex) {
			KdMatrix matrix_01 = {};
			matrix_01.RotateZ(camera_rotateY + (body_rotateY * -1));
			matrix_01.Move(KdVector3(-540.0f, -260.0f, 0.0f));
			SHADER.m_spriteShader.SetMatrix(matrix_01);
			SHADER.m_spriteShader.DrawTex(m_spBodyTex.get(), 0, 0);
		}

		// 砲塔のY軸回転
		KdMatrix turret_matrix = m_owner.GetTankParts()->GetMainGun()->GetMatrix();
		const float turret_rotateY = atan2f(turret_matrix.GetAxisZ().x, turret_matrix.GetAxisZ().z) * -1;

		// 砲塔
		if (m_spTurretTex) {
			KdMatrix matrix_02 = {};
			matrix_02.RotateZ(turret_rotateY - (camera_rotateY * -1));
			matrix_02.Move(KdVector3(-540.0f, -260.0f, 0.0f));
			SHADER.m_spriteShader.SetMatrix(matrix_02);
			SHADER.m_spriteShader.DrawTex(m_spTurretTex.get(), 0, 0);
		}

		// UI
		if (m_spBlackRect01Tex) {
			SHADER.m_spriteShader.SetMatrix(KdMatrix());
			SHADER.m_spriteShader.DrawTex(m_spBlackRect01Tex.get(), -540, -260);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: テクスチャ読み込み
//-----------------------------------------------------------------------------
void PlayerInterface::LoadTexture()
{
	m_spThirdAimTex			= KdResFac.GetTexture("Data/Texture/TankInterface/ThirdAim.png");
	m_spFirstAim01Tex		= KdResFac.GetTexture("Data/Texture/TankInterface/FirstAim01.png");
	m_spFirstAim02Tex		= KdResFac.GetTexture("Data/Texture/TankInterface/FirstAim02.png");
	m_spFirstAim03Tex		= KdResFac.GetTexture("Data/Texture/TankInterface/FirstAim03.png");

	m_spBodyTex				= KdResFac.GetTexture("Data/Texture/TankInterface/Body.png");
	m_spTurretTex			= KdResFac.GetTexture("Data/Texture/TankInterface/Turret.png");
	m_spApBulletTex			= KdResFac.GetTexture("Data/Texture/TankInterface/ApBullet.png");
	m_spFirstVisibilityTex	= KdResFac.GetTexture("Data/Texture/TankInterface/FirstVisibility.png");
	m_spThirdVisibilityTex	= KdResFac.GetTexture("Data/Texture/TankInterface/ThirdVisibility.png");

	m_spBlackRect01Tex		= KdResFac.GetTexture("Data/Texture/TankInterface/BlackRect01.png");
	m_spBlackRect02Tex		= KdResFac.GetTexture("Data/Texture/TankInterface/BlackRect02.png");
	m_spFillRect01Tex		= KdResFac.GetTexture("Data/Texture/TankInterface/FillRect01.png");
	m_spFillRect02Tex		= KdResFac.GetTexture("Data/Texture/TankInterface/FillRect02.png");
	m_spFillRect03Tex		= KdResFac.GetTexture("Data/Texture/TankInterface/FillRect03.png");

	m_spRedTriangleTex		= KdResFac.GetTexture("Data/Texture/TankInterface/RedTriangle.png");
	m_spGreenTriangleTex	= KdResFac.GetTexture("Data/Texture/TankInterface/GreenTriangle.png");

	m_spNumberTexArray[0]	= KdResFac.GetTexture("Data/Texture/TankInterface/Number/0.png");
	m_spNumberTexArray[1]	= KdResFac.GetTexture("Data/Texture/TankInterface/Number/1.png");
	m_spNumberTexArray[2]	= KdResFac.GetTexture("Data/Texture/TankInterface/Number/2.png");
	m_spNumberTexArray[3]	= KdResFac.GetTexture("Data/Texture/TankInterface/Number/3.png");
	m_spNumberTexArray[4]	= KdResFac.GetTexture("Data/Texture/TankInterface/Number/4.png");
	m_spNumberTexArray[5]	= KdResFac.GetTexture("Data/Texture/TankInterface/Number/5.png");
	m_spNumberTexArray[6]	= KdResFac.GetTexture("Data/Texture/TankInterface/Number/6.png");
	m_spNumberTexArray[7]	= KdResFac.GetTexture("Data/Texture/TankInterface/Number/7.png");
	m_spNumberTexArray[8]	= KdResFac.GetTexture("Data/Texture/TankInterface/Number/8.png");
	m_spNumberTexArray[9]	= KdResFac.GetTexture("Data/Texture/TankInterface/Number/9.png");
}

//-----------------------------------------------------------------------------
// Name: UpdateReload()
// Desc: リロード更新 ※修正.マジックナンバー
//-----------------------------------------------------------------------------
void PlayerInterface::UpdateReload()
{
	// リロードが開始された
	if (m_owner.GetReloadCount() == m_owner.GetReloadCountMax()) {
		m_bulletTexturePos = KdVector3(730.0f, -360.0f, 0.0f);
		m_reloadState = ReloadState::eStart;
	}

	// リロードの状態(進捗)によって更新(テクスチャの移動)
	switch (m_reloadState)
	{
		// リロード開始
	case ReloadState::eStart:
		// 画面外の下から徐々に出てくる
		m_bulletTexturePos.y += 2.0f;
		// 特定の座標で遷移.eCenter
		if (m_bulletTexturePos.y >= -300) { m_reloadState = ReloadState::eCenter; }
		break;

		// リロード中盤
	case ReloadState::eCenter:
		// 出てきたら徐々に右に移動する
		m_bulletTexturePos.x -= 2.0f;
		// 特定の座標で遷移.eEnd
		if (m_bulletTexturePos.x <= 500) { m_reloadState = ReloadState::eEnd; }
		break;

		// リロード完了
	case ReloadState::eEnd:
		break;
	}
}

//-----------------------------------------------------------------------------
// Name: DrawSpriteStateMove()
// Desc: 3人称時の2D描画
//-----------------------------------------------------------------------------
void PlayerInterface::DrawSpriteStateMove()
{
	// 3人称時の照準器
	if (m_spThirdAimTex) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spThirdAimTex.get(), 0, -20);
	}

	//--------------------------------------------------
	// 頭上にマーク描画
	//--------------------------------------------------

	// 画面の幅と高さ設定
	const int screenWidth  = D3D.GetBackBuffer()->GetWidth();
	const int screenHeight = D3D.GetBackBuffer()->GetHeight();

	// ビューポート変換行列
	KdMatrix viewportMat = {};
	viewportMat._11 =  screenWidth  * 0.5f;
	viewportMat._22 = -screenHeight * 0.5f;
	viewportMat._41 =  screenWidth  * 0.5f;
	viewportMat._42 =  screenHeight * 0.5f;

	// 各カメラ行列
	KdMatrix viewMat = m_owner.GetCameraComponent()->GetViewMatrix();
	KdMatrix projMat = m_owner.GetCameraComponent()->GetProjMatrix();

	// 全Object
	for (auto& object : OBJ_MAGER.GetTankList()) {
		// 自身は無視
		if (object->GetTag() & OBJECT_TAG::TAG_Player) { continue; }

		// 味方はすぐに描画
		if (object->GetTag() & OBJECT_TAG::TAG_Allies) {
			if (!object->IsAlive()) { continue; }
			// 描画
			DrawSpriteOverheadMark(screenHeight, screenWidth, viewportMat, viewMat, projMat, object);
		}
		
		// 敵は発見確認後描画
		if (object->GetTag() & OBJECT_TAG::TAG_Enemy) {
			if (!object->IsAlive()) { continue; }
			if (!object->IsDiscovery()) { continue; }
			// 描画
			DrawSpriteOverheadMark(screenHeight, screenWidth, viewportMat, viewMat, projMat, object);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: DrawSpriteAimMove()
// Desc: 1人称時の2D描画
//-----------------------------------------------------------------------------
void PlayerInterface::DrawSpriteAimMove()
{
	// 周りの黒い丸
	if (m_spFirstAim01Tex) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spFirstAim01Tex.get(), 0, 0);
	}

	// 三角形のやつ ※中心の
	if (m_spFirstAim03Tex) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spFirstAim03Tex.get(), 0, -80);
	}

	// 周りの黒い点々
	if (m_spFirstAim02Tex) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spFirstAim02Tex.get(), 0, 0);
	}
}

//-----------------------------------------------------------------------------
// Name: DrawSpriteOverheadMark()
// Desc: 戦車の頭上にマーク
//-----------------------------------------------------------------------------
void PlayerInterface::DrawSpriteOverheadMark(int screenH, int screenW, KdMatrix viewportMat, KdMatrix viewMat, KdMatrix projMat, const std::shared_ptr<GameObject> object)
{
	//--------------------------------------------------
	// 視推台の中か確認
	//--------------------------------------------------

	// 対象のメッシュ所得
	KdMesh* pMesh = object->GetCollModelComponent()->GetMesh(0).get();
	if (pMesh == nullptr) { return; }

	// メッシュのAABBから物体の行列も考慮してOBBへ変換
	DirectX::BoundingOrientedBox obb = {};
	DirectX::BoundingOrientedBox::CreateFromBoundingBox(obb, pMesh->GetBoundingBox());
	obb.Transform(obb, object->GetMatrix());

	//--------------------------------------------------
	// 3D -> 2D変換
	//--------------------------------------------------

	// 3D->2D変換行列 ※参考: http://rudora7.blog81.fc2.com/blog-entry-415.html
	KdMatrix convertMat = object->GetMatrix() * viewMat * projMat * viewportMat;

	// 表示する座標
	KdVector3 resultPos = {};
	resultPos.TransformCoord(convertMat);
	int textureX = (static_cast<int>(resultPos.x) - screenW / 2);
	int textureY = (static_cast<int>(resultPos.y) - screenH / 2) * -1;// SpriteShaderの反転に対応

	//--------------------------------------------------
	// 左右どっちか確認
	//--------------------------------------------------

	// 対象への方向
	KdVector3 vecA = object->GetMatrix().GetTranslation() - m_owner.GetMatrix().GetTranslation();
	// カメラのZ軸
	KdVector3 vecB = m_owner.GetCameraComponent()->GetCameraMatrix().GetAxisZ();
	// 角度所得
	float angle = 0;
	KdVector3::CalcAngle2(Math::Vector2(vecA.x, vecA.z), Math::Vector2(vecB.x, vecB.z), angle);

	// true ... 右に表示
	const bool isRight = (angle > 0);

	//--------------------------------------------------
	// マークの描画
	//--------------------------------------------------

	// TAG確認 -> 使用するテクスチャ確定
	auto spTexture = (object->GetTag() & OBJECT_TAG::TAG_Enemy)
		? m_spRedTriangleTex.get()		// 0 ... Red.Enemy
		: m_spGreenTriangleTex.get();	// 1 ... Green.Player

	// 視推台の範囲内か
	if (m_owner.GetCameraComponent()->GetFrustum().Intersects(obb)) {
		// 行列設定
		KdMatrix transMat = {};
		transMat.CreateTranslation(static_cast<float>(textureX), 260, 0);
		// 描画
		SHADER.m_spriteShader.SetMatrix(transMat);
		SHADER.m_spriteShader.DrawTex(spTexture, 0, 0);
	}
	else {
		// 行列設定
		KdMatrix rotMat, transMat;
		if (isRight) {
			transMat.CreateTranslation(600, 260, 0);
			rotMat.CreateRotationX(0);
			rotMat.RotateZ(90 * KdToRadians);
		}
		else {
			transMat.CreateTranslation(-600, 260, 0);
			rotMat.CreateRotationX(0);
			rotMat.RotateZ(-90 * KdToRadians);
		}
		transMat = rotMat * transMat;

		// 描画
		SHADER.m_spriteShader.SetMatrix(transMat);
		SHADER.m_spriteShader.DrawTex(spTexture, 0, 0);
	}

	//--------------------------------------------------
	// 距離の描画 ※Enemyのみ
	//--------------------------------------------------
	if (m_owner.GetCameraComponent()->GetFrustum().Intersects(obb) && (object->GetTag() & OBJECT_TAG::TAG_Enemy)) {
		// 対象との距離
		int distance = static_cast<int>(KdVector3::GetDistance(m_owner.GetMatrix().GetTranslation(), object->GetMatrix().GetTranslation()));
		// 各位を算出
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spNumberTexArray[(distance / 100) % 10].get(), textureX - 22, 300);
		SHADER.m_spriteShader.DrawTex(m_spNumberTexArray[(distance / 10) % 10].get(), textureX + 0, 300);
		SHADER.m_spriteShader.DrawTex(m_spNumberTexArray[(distance / 1) % 10].get(), textureX + 22, 300);
	}
}