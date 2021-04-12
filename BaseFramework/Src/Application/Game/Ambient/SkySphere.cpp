//-----------------------------------------------------------------------------
// File: SkySphere.cpp
//
// Edit: 2021/03/17 野筒隼輔
//-----------------------------------------------------------------------------
#include "SkySphere.h"
#include "Application/Component/ModelComponent.h"

//-----------------------------------------------------------------------------
// Name: SkySphere()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
SkySphere::SkySphere()
	: m_rotateSpeed(0.004f * KdToRadians)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void SkySphere::Deserialize(const json11::Json& json_object)
{
	GameObject::Deserialize(json_object);
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void SkySphere::Update()
{
	KdMatrix rotateMatrix = {};
	rotateMatrix.CreateRotationX(0.0f);
	rotateMatrix.RotateY(m_rotateSpeed);

	m_worldMatrix = rotateMatrix * m_worldMatrix;
}

//-----------------------------------------------------------------------------
// Name: Draw()
// Desc: 描画
//-----------------------------------------------------------------------------
void SkySphere::Draw()
{
	// EeffectShaderを使用
	SHADER.m_effectShader.SetToDevice();
	SHADER.m_effectShader.SetWorldMatrix(m_worldMatrix);

	SHADER.m_effectShader.DrawMesh(m_spDrawModelComponent->GetMesh(0).get(), m_spDrawModelComponent->GetModel()->GetMaterials());

	// 設定を元に戻す
	SHADER.m_modelShader.SetToDevice();
}