//-----------------------------------------------------------------------------
// File: FixedCamera.cpp
//
// Edit: 2021/03/29 野筒隼輔
//-----------------------------------------------------------------------------
#include "FixedCamera.h"
#include "Application/Component/CameraComponent.h"
#include "Application/Manager/CameraManager.h"

//-----------------------------------------------------------------------------
// Name: FixedCamera()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
FixedCamera::FixedCamera()
	: m_viewPosition(KdVector3(0.0f, 0.0f, 0.0f))
	, m_prevMousePos()
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void FixedCamera::Deserialize(const json11::Json& json_object)
{
	// 基底クラスの初期化(共通の初期化)
	GameObject::Deserialize(json_object);
	// 注視点設定
	m_viewPosition = m_worldMatrix.GetTranslation();

	//--------------------------------------------------
	// コンポーネント作成
	//--------------------------------------------------

	//カメラ
	m_spCameraComponent = std::make_shared<CameraComponent>(*this);
	if (m_spCameraComponent) {
		// タイトルで視覚効果を持たせるために視野角を20.0fに変更
		m_spCameraComponent->SetFovAngle(20.0f * KdToRadians);
		CAMERA_MAGER.SetTargetCamera(m_spCameraComponent);
	}
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void FixedCamera::Update()
{
	if (m_spCameraComponent == nullptr) { return; }

	// 注視点更新
	m_worldMatrix.SetTranslation(m_viewPosition);
	// カメラ更新
	m_spCameraComponent->SetCameraMatrix(m_worldMatrix);
}