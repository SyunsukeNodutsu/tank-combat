//-----------------------------------------------------------------------------
// File: ModelComponent.cpp
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#include "ModelComponent.h"
#include "CameraComponent.h"
#include "Application/Game/Scene/Scene.h"
#include "Application/Manager/CameraManager.h"
#include "Application/Manager/DebugManager.h"
#include "Application/Manager/DrawManager.h"

//-----------------------------------------------------------------------------
// Name: ModelComponent()
// Desc: コンストラクタ 所持者(オーナー)の設定
//-----------------------------------------------------------------------------
ModelComponent::ModelComponent(GameObject& owner)
	: m_owner(owner)
	, m_spModel(nullptr)
	, m_coppiedNodes()
	, m_uvOffset(Math::Vector2(0.0f, 0.0f))
	, m_uvMoveNum(Math::Vector2(0.0f, 0.0f))
	, m_isEnable(true)
	, m_isUvScroll(false)
{
}

//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: 3D描画
//-----------------------------------------------------------------------------
void ModelComponent::Draw()
{
	if ((m_isEnable == false) || (m_spModel == nullptr)) { return; }

	// 現在使用中のカメラ
	auto camera = CAMERA_MAGER.GetTargetCamera().lock();
	if (camera == nullptr) { return; }

	//--------------------------------------------------
	// シェーダオプション
	//--------------------------------------------------
	{
		// UVスクロール
		if (m_isUvScroll) {
			m_uvOffset += m_uvMoveNum;
			SHADER.m_modelShader.SetUvOffset(m_uvOffset);
		}
		else { SHADER.m_modelShader.SetUvOffset(Math::Vector2::Zero); }
	}

	//--------------------------------------------------
	// 通常描画
	//--------------------------------------------------
	for (UINT i = 0; i < m_coppiedNodes.size(); i++)
	{
		// ノード所得
		auto& node = m_coppiedNodes[i];
		if (node.m_spMesh == nullptr) { continue; }

		//--------------------------------------------------
		// メッシュのAABBから物体の行列も考慮してOBBへ変換
		//--------------------------------------------------
		DirectX::BoundingOrientedBox obb = {};
		DirectX::BoundingOrientedBox::CreateFromBoundingBox(obb, node.m_spMesh.get()->GetBoundingBox());
		obb.Transform(obb, (node.m_localTransform * m_owner.GetMatrix()));// ローカル * ワールド

		//--------------------------------------------------
		// カリング実行
		//--------------------------------------------------

		// 視推台の範囲内
		if (!camera->GetFrustum().Intersects(obb)) { continue; }

		// 発表用にOBB描画
		if (DEBUG_MAGER.IsOBBShow()) { DrawOBB(obb); }

		// ModelShader使用
		SHADER.m_modelShader.SetWorldMatrix(node.m_localTransform * m_owner.GetMatrix());
		SHADER.m_modelShader.DrawMesh(node.m_spMesh.get(), m_spModel->GetMaterials());
	}
}

//-----------------------------------------------------------------------------
// Name: DrawShadowMap()
// Desc: シャドウマップ描画
//-----------------------------------------------------------------------------
void ModelComponent::DrawShadowMap()
{
	if ((m_isEnable == false) || (m_spModel == nullptr)) { return; }

	// 全ノード
	for (UINT i = 0; i < m_coppiedNodes.size(); i++)
	{
		// ノード所得
		auto& node = m_coppiedNodes[i];
		if (node.m_spMesh == nullptr) { continue; }

		// GenShadowMapShader使用 ※カリングする場合は伸びる影も考慮
		SHADER.m_genShadowMapShader.SetWorldMatrix(node.m_localTransform * m_owner.GetMatrix());
		SHADER.m_genShadowMapShader.DrawMeshDepth(node.m_spMesh.get(), m_spModel->GetMaterials());
	}
}

//-----------------------------------------------------------------------------
// Name: DrawEffect()
// Desc: エフェクト描画
//-----------------------------------------------------------------------------
void ModelComponent::DrawEffect()
{
	if ((m_isEnable == false) || (m_spModel == nullptr)) { return; }

	// 現在使用中のカメラ
	auto camera = CAMERA_MAGER.GetTargetCamera().lock();
	if (camera == nullptr) { return; }

	for (UINT i = 0; i < m_coppiedNodes.size(); i++)
	{
		auto& node = m_coppiedNodes[i];
		if (node.m_spMesh == nullptr) { continue; }

		//--------------------------------------------------
		// メッシュのAABBから物体の行列も考慮してOBBへ変換
		//--------------------------------------------------
		DirectX::BoundingOrientedBox obb = {};
		DirectX::BoundingOrientedBox::CreateFromBoundingBox(obb, node.m_spMesh.get()->GetBoundingBox());
		obb.Transform(obb, (node.m_localTransform * m_owner.GetMatrix()));// ローカル * ワールド

		//--------------------------------------------------
		// カリング実行
		//--------------------------------------------------

		// 視推台の範囲内
		if (!camera->GetFrustum().Intersects(obb)) { continue; }

		// EffectShader使用
		SHADER.m_effectShader.SetWorldMatrix(node.m_localTransform * m_owner.GetMatrix());
		SHADER.m_effectShader.DrawMesh(node.m_spMesh.get(), m_spModel->GetMaterials());
	}
}

//-----------------------------------------------------------------------------
// Name: SetModel()
// Desc: モデルの設定
//-----------------------------------------------------------------------------
void ModelComponent::SetModel(const std::shared_ptr<KdModel>& model)
{
	if (model == nullptr) { return; }
	m_spModel = model;
	m_coppiedNodes.clear();
	// コンテナの中身全部コピー
	m_coppiedNodes = model->GetOriginalNodes();
}

//-----------------------------------------------------------------------------
// Name: DrawOBB()
// Desc: バウンディングボックス描画
//-----------------------------------------------------------------------------
void ModelComponent::DrawOBB(const DirectX::BoundingOrientedBox& obb)
{
	// コーナーの所得
	KdVector3 obbCorners[obb.CORNER_COUNT];
	obb.GetCorners(obbCorners);

	DRAW_MAGER.AddDebugLine(obbCorners[0], obbCorners[1], kBlackColor);
	DRAW_MAGER.AddDebugLine(obbCorners[1], obbCorners[2], kBlackColor);
	DRAW_MAGER.AddDebugLine(obbCorners[2], obbCorners[3], kBlackColor);
	DRAW_MAGER.AddDebugLine(obbCorners[3], obbCorners[0], kBlackColor);

	DRAW_MAGER.AddDebugLine(obbCorners[4], obbCorners[5], kBlackColor);
	DRAW_MAGER.AddDebugLine(obbCorners[5], obbCorners[6], kBlackColor);
	DRAW_MAGER.AddDebugLine(obbCorners[6], obbCorners[7], kBlackColor);
	DRAW_MAGER.AddDebugLine(obbCorners[7], obbCorners[4], kBlackColor);

	DRAW_MAGER.AddDebugLine(obbCorners[0], obbCorners[4], kBlackColor);
	DRAW_MAGER.AddDebugLine(obbCorners[1], obbCorners[5], kBlackColor);
	DRAW_MAGER.AddDebugLine(obbCorners[2], obbCorners[6], kBlackColor);
	DRAW_MAGER.AddDebugLine(obbCorners[3], obbCorners[7], kBlackColor);
}