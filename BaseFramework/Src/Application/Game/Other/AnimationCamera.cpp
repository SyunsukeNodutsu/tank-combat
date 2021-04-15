//-----------------------------------------------------------------------------
// File: AnimationCamera.cpp
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#include "AnimationCamera.h"

#include "Application/Game/Scene/Scene.h"
#include "Application/Component/CameraComponent.h"
#include "Application/Manager/CameraManager.h"
#include "Application/Manager/DrawManager.h"

//-----------------------------------------------------------------------------
// Name: AnimationCamera()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
AnimationCamera::AnimationCamera()
	: m_wpStart()
	, m_wpEnd()
	, m_progress(0.0f)
	, m_speed(0.01f)
{
	m_name = "AnimationCamera";
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void AnimationCamera::Update()
{
	if (m_spCameraComponent == nullptr) { return; }

	// 補完する対象がどちらも存在するか
	std::shared_ptr<GameObject> start = m_wpStart.lock(), end = m_wpEnd.lock();
	if (!start || !end) {
		m_isEnable = false;
		return;
	}

	// 補完する行列の所得
	auto& startMat = start->GetCameraComponent()->GetCameraMatrix();
	auto& endMat = end->GetCameraComponent()->GetCameraMatrix();

	// 座標の補完
	const KdVector3& startPos = startMat.GetTranslation();
	const KdVector3& endPos = endMat.GetTranslation();
	KdVector3 vTo = endPos - startPos;				// ゴール地点へのベクトル
	KdVector3 vNow = startPos + vTo * m_progress;	// 進行具合を加味して座標を所得

	// 開始地点と終了地点のクォータニオンを作成(行列 -> 回転)
	DirectX::XMVECTOR qSt = DirectX::XMQuaternionRotationMatrix(startMat);
	DirectX::XMVECTOR qEd = DirectX::XMQuaternionRotationMatrix(endMat);

	// クォータニオンを使用して回転の保管 OTW ... オンザウェイ
	DirectX::XMVECTOR qOTW = DirectX::XMQuaternionSlerp(qSt, qEd, m_progress);

	// クォータニオンを回転行列に変換
	KdMatrix rotMat = DirectX::XMMatrixRotationQuaternion(qOTW);

	// カメラに設定する行列
	KdMatrix cameraMat = rotMat;
	cameraMat.SetTranslation(vNow);

	// カメラへ設定
	m_spCameraComponent->SetCameraMatrix(cameraMat);

	// 進行具合を更新
	m_progress += m_speed;
	if (m_progress > 1.0f) {
		m_progress = 1.0f;
		m_isEnable = false;
		// ゴール地点まで補完し終わったので、カメラターゲットをゴール地点のキャラクタにする
		CAMERA_MAGER.SetTargetCamera(end->GetCameraComponent());
	}
}

//-----------------------------------------------------------------------------
// Name: SetInfo()
// Desc: 情報設定 ※初期設定
//-----------------------------------------------------------------------------
void AnimationCamera::SetInfo(const std::shared_ptr<GameObject>& start, const std::shared_ptr<GameObject>& end, float speed)
{
	if (!start->GetCameraComponent() || !end->GetCameraComponent()) return;

	// 情報設定
	m_wpStart	= start;
	m_wpEnd		= end;
	m_progress	= 0.0f;
	m_speed		= speed;

	// カメラ生成
	m_spCameraComponent = std::make_shared<CameraComponent>(*this);
	if (m_spCameraComponent) {
		// 視野角設定
		m_spCameraComponent->SetFovAngle(120.0f * KdToRadians);
	}

	// ぼかしOFF
	DRAW_MAGER.SetBlurValue(Math::Vector2(0.0f, 0.0f));
}