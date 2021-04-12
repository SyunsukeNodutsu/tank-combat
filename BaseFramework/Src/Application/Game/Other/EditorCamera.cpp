//-----------------------------------------------------------------------------
// File: Editorcamera.cpp
//
// Edit: 2021/03/08 野筒隼輔
//-----------------------------------------------------------------------------
#include "EditorCamera.h"
#include "Application/main.h"
#include "Application/Game/Scene/Scene.h"
#include "Application/Game/Scene/Pause.h"
#include "Application/Component/CameraComponent.h"
#include "Application/Manager/CameraManager.h"

//-----------------------------------------------------------------------------
// Name: EditorCamera()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
EditorCamera::EditorCamera()
	: m_viewPosition(KdVector3(0.0f, 0.0f, 0.0f))
	, m_prevMousePos({ 0, 0 })
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void EditorCamera::Deserialize(const json11::Json& json_object)
{
	// 基底クラスの初期化(共通の初期化)
	GameObject::Deserialize(json_object);
	// 注視点設定
	m_viewPosition = m_worldMatrix.GetTranslation();

	//カメラ
	m_spCameraComponent = std::make_shared<CameraComponent>(*this);
	if (m_spCameraComponent) {
		m_spCameraComponent->SetFovAngle(60.0f * KdToRadians);
		CAMERA_MAGER.SetTargetCamera(m_spCameraComponent);
	}
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void EditorCamera::Update()
{
	if (PAUSE.IsEnable()) { return; }

	// 現在のマウス座標所得
	POINT mousePos = {};
	GetCursorPos(&mousePos);

	//--------------------------------------------------
	// 回転量や移動量を操作
	//--------------------------------------------------

	// 各値にかけて増減
	float raito = 1.0f;

	// 増減させる
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)	raito = 5.0f;// shiftで大きく
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)	raito = 0.2f;// controlで小さく

	//--------------------------------------------------
	// カメラの回転 ※座標を{0,0,0}にしてその場で回転
	//--------------------------------------------------

	// マウスの右ボタン
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		// マウスの前回からの移動量
		float deltaX = static_cast<float>(mousePos.x - m_prevMousePos.x) * 0.5f * raito;
		float deltaY = static_cast<float>(mousePos.y - m_prevMousePos.y) * 0.5f * raito;

		// 座標を{0,0,0}にしてから回転
		KdVector3 pos = m_worldMatrix.GetTranslation();
		m_worldMatrix.SetTranslation({ 0,0,0 });

		// カメラY回転
		KdMatrix mRotateY = {};
		mRotateY.CreateRotationX(0.0f);
		mRotateY.RotateY(deltaX * KdToRadians);
		m_worldMatrix *= mRotateY;

		// カメラX回転
		KdMatrix mRotateX = {};
		mRotateX.CreateRotationAxis(m_worldMatrix.GetAxisX(), deltaY * KdToRadians);
		m_worldMatrix *= mRotateX;

		// 座標を戻す
		m_worldMatrix.SetTranslation(pos);
	}

	//--------------------------------------------------
	// カメラの移動
	//--------------------------------------------------

	// マウスのホイールドラッグ ※ホイール押し込み
	if (GetAsyncKeyState(VK_MBUTTON) & 0x8000)
	{
		float deltaX = static_cast<float>(mousePos.x - m_prevMousePos.x) * 0.05f * raito;
		float deltaY = static_cast<float>(mousePos.y - m_prevMousePos.y) * 0.05f * raito;
		
		m_viewPosition -= m_worldMatrix.GetAxisX() * deltaX;
		m_viewPosition += m_worldMatrix.GetAxisZ() * deltaY;
		m_worldMatrix.SetTranslation(m_viewPosition);
	}

	// 前フレームのマウス座標
	m_prevMousePos = mousePos;

	// カメラ更新
	if (m_spCameraComponent) { m_spCameraComponent->SetCameraMatrix(m_worldMatrix); }
}