//-----------------------------------------------------------------------------
// File: CameraComponent.cpp
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#include "CameraComponent.h"

//-----------------------------------------------------------------------------
// Name: CameraComponent()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
CameraComponent::CameraComponent(GameObject& owner)
	: m_owner(owner)
	, m_offsetMatrix()
	, m_cameraMatrix()
	, m_viewMatrix()
	, m_projMatrix()
{
	// 射影行列作成
	constexpr float fov_angle = 60.0f * KdToRadians;
	m_projMatrix.CreateProjectionPerspectiveFov(fov_angle, D3D.GetBackBuffer()->GetAspectRatio(), 0.01f, 5000.0f);
}

//-----------------------------------------------------------------------------
// Name: SetToShader()
// Desc: デバイスへ送信
//-----------------------------------------------------------------------------
void CameraComponent::SetToShader()
{
	// Effecseer View行列 射影行列設定
	EFFEKSEER.SetCameraMatrix(m_viewMatrix, m_projMatrix);

	// 追従カメラ
	SHADER.m_cb7_Camera.Work().CamPos = m_cameraMatrix.GetTranslation();
	// view行列
	SHADER.m_cb7_Camera.Work().mV = m_viewMatrix;
	// proj行列
	SHADER.m_cb7_Camera.Work().mP = m_projMatrix;
	// シェーダーの定数バッファに設定
	SHADER.m_cb7_Camera.Write();
}

//-----------------------------------------------------------------------------
// Name: CreateRayInfoFromPlanePos()
// Desc: カメラから見た指定された座標にレイを飛ばす際の情報設定
//-----------------------------------------------------------------------------
void CameraComponent::CreateRayInfoFromPlanePos(RayInfo& dst_info, const Math::Vector2 plane_pos)
{
	// 各行列を逆行列計算
	KdMatrix invView = m_viewMatrix;
	invView.Inverse();
	KdMatrix invProj = m_projMatrix;
	invProj.Inverse();

	// 画面の幅と高さ設定
	int screenWidth  = D3D.GetBackBuffer()->GetWidth();
	int screenHeight = D3D.GetBackBuffer()->GetHeight();

	// ビューポート変換行列
	KdMatrix invViewport = {};
	invViewport._11 = screenWidth	* 0.5f;
	invViewport._22 = -screenHeight * 0.5f;
	invViewport._41 = screenWidth	* 0.5f;
	invViewport._42 = screenHeight	* 0.5f;

	// ビューポートの逆行列も計算
	invViewport.Inverse();

	// 2D->3D座標変換行列作成
	KdMatrix convertMat = invViewport * invProj * invView;

	// 最近点と最遠点の座標設定
	KdVector3 nearPos = KdVector3(plane_pos.x, plane_pos.y, 0.0f);
	KdVector3 forPos = KdVector3(plane_pos.x, plane_pos.y, 1.0f);

	// 3D座標へ変換
	nearPos.TransformCoord(convertMat);
	forPos.TransformCoord(convertMat);

	// レイ情報設定
	dst_info.m_pos		= nearPos;					// 発射地点は手前から
	dst_info.m_dir		= forPos - nearPos;			// 手前から奥に
	dst_info.m_maxRange = dst_info.m_dir.Length();	// 判定距離は最近点と最遠点の長さ分
	dst_info.m_dir.Normalize();						// 方向ベクトルは単位化
}

//-----------------------------------------------------------------------------
// Name: SetCameraMatrix()
// Desc: カメラ.ビュー行列設定
//-----------------------------------------------------------------------------
void CameraComponent::SetCameraMatrix(const KdMatrix& matrix)
{
	// カメラ情報セット
	m_cameraMatrix = m_offsetMatrix * matrix;

	// カメラ行列からビュー行列を算出
	m_viewMatrix = m_cameraMatrix;
	m_viewMatrix.Inverse();

	// 視推台作成
	KdQuaternion quaternion = DirectX::XMQuaternionRotationMatrix(m_cameraMatrix);
	DirectX::BoundingFrustum::CreateFromMatrix(m_frustum, m_projMatrix);
	m_frustum.Origin = m_cameraMatrix.GetTranslation();
	m_frustum.Orientation = quaternion;
}