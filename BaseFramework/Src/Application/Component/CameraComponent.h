//-----------------------------------------------------------------------------
// File: CameraComponent.h
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"
#include "CollisionComponent.h"

//-----------------------------------------------------------------------------
// Name: class CameraComponent
// Desc: 追従型のカメラクラス ※修正.最望遠距離とか視野角はメンバ変数に
//-----------------------------------------------------------------------------
class CameraComponent
{
public:

	// コンストラクタ
	CameraComponent(GameObject& owner);

	// カメラ情報(ビュー,射影行列など)をシェーダー設定
	void SetToShader();

	// カメラから見た指定された座標にレイを飛ばす際の情報設定
	void CreateRayInfoFromPlanePos(RayInfo& dst_info, const Math::Vector2 plane_pos);

	//--------------------------------------------------
	//  所得
	//--------------------------------------------------
	// カメラ.ビュー行列設定
	void SetCameraMatrix(const KdMatrix& matrix);
	// 視野角の設定(引数...ラジアン)
	void SetFovAngle(const float angle) { m_projMatrix.CreateProjectionPerspectiveFov(angle, D3D.GetBackBuffer()->GetAspectRatio(), 0.01f, 5000.0f); }
	// Offset行列所得
	inline KdMatrix& OffsetMatrix() { return m_offsetMatrix; }
	// カメラ行列所得
	inline const KdMatrix& GetCameraMatrix() const { return m_cameraMatrix; }
	// ビュー行列所得
	inline const KdMatrix& GetViewMatrix() const { return m_viewMatrix; }
	// 射影行列所得
	inline const KdMatrix& GetProjMatrix() const { return m_projMatrix; }
	// 視推台所得
	inline const DirectX::BoundingFrustum GetFrustum() const { return m_frustum; }

protected:
	GameObject&		m_owner;			// オーナー
	KdMatrix		m_offsetMatrix;		// オフセット行列
	KdMatrix		m_cameraMatrix;		// カメラ行列
	KdMatrix		m_viewMatrix;		// ビュー行列
	KdMatrix		m_projMatrix;		// 射影行列
	DirectX::BoundingFrustum m_frustum; // 視推台
};