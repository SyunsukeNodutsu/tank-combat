//-----------------------------------------------------------------------------
// File: GameObject.cpp
//
// Edit: 2021/02/11 野筒隼輔
//-----------------------------------------------------------------------------
#include "GameObject.h"

#include "Application/Component/CameraComponent.h"
#include "Application/Component/CollisionComponent.h"
#include "Application/Component/InputComponent.h"
#include "Application/Component/ModelComponent.h"

GameObject::GameObject()
	: m_worldMatrix()
	, m_prevMatrix()
	, m_className("GameObject")
	, m_name("Object")
	, m_modelName("None")
	, m_collisionModelName("None")
	, m_prefabFilepath("None")
	, m_tag(OBJECT_TAG::TAG_None)
	, m_colRadius(1.0f)
	, m_isAlive(true)
	, m_isEnable(true)
	, m_spCameraComponent(nullptr)
	, m_spCollisionComponent(nullptr)
	, m_spInputComponent(nullptr)
	, m_spDrawModelComponent(nullptr)
	, m_spCollisionModelComponent(nullptr)
{
}

GameObject::~GameObject() { Release(); }

void GameObject::Deserialize(const json11::Json& json_object)
{
	//--------------------------------------------------
	// コンポーネントの生成
	//--------------------------------------------------
	// 衝突判定
	m_spCollisionComponent = std::make_shared<CollisionComponent>(*this);
	// 描画用モデル
	m_spDrawModelComponent = std::make_shared<ModelComponent>(*this);
	// 衝突判定用モデル
	m_spCollisionModelComponent = std::make_shared<ModelComponent>(*this);

	//--------------------------------------------------
	// Jsonから行列を作成
	//--------------------------------------------------
	TransformSetting(json_object);

	//--------------------------------------------------
	// Jsonから情報を設定
	//--------------------------------------------------
	ParameterSetting(json_object);
}

void GameObject::ImGuiUpdate()
{
	// 名前
	ImGui::InputText("Name", &m_name);
	ImGui::InputText("ModelName", &m_modelName);
	ImGui::InputText("Prefab", &m_prefabFilepath);

	// 90度回転
	if (ImGui::Button("RotLeft 90"))
	{
		KdMatrix rotMat = {};
		rotMat.CreateRotationX(0.0f);
		rotMat.RotateY(90 * KdToRadians);
		m_worldMatrix = rotMat * m_worldMatrix;
	}

	// Transform
	if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		KdVector3 position = m_worldMatrix.GetTranslation();
		KdVector3 rotation = m_worldMatrix.GetAngle() * KdToDegrees;
		KdVector3 scaling = m_worldMatrix.GetScale();

		if (ImGui::DragFloat3("Position", &position.x, 0.01f))
		{
			m_worldMatrix.SetTranslation(position);
		}
		ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
		ImGui::DragFloat3("Scaling", &scaling.x, 0.1f);

		if (ImGui::Button("copy transform to json"))
		{
			std::string string = KdFormat("\"Position\": [%.1f, %.1f, %.1f],\n", position.x, position.y, position.z);
			string += KdFormat("\"Rotation\": [%.1f, %.1f, %.1f],\n", rotation.x, rotation.y, rotation.z);
			string += KdFormat("\"Scaling\": [%.1f, %.1f, %.1f],\n", scaling, scaling, scaling);
			ImGui::SetClipboardText(string.c_str());
		}

		ImGui::TreePop();
	}
}

void GameObject::Draw()
{
	if (m_spDrawModelComponent == nullptr) return;
	m_spDrawModelComponent->Draw();
}

void GameObject::DrawShadowMap()
{
	if (m_spDrawModelComponent == nullptr) return;
	m_spDrawModelComponent->DrawShadowMap();
}

KdMatrix GameObject::GetOneMove()
{
	KdMatrix InverseMat = m_prevMatrix;
	InverseMat.Inverse();
	return InverseMat * m_worldMatrix;
}

void GameObject::Release()
{
	// 各コンポーネント所有権の破棄
	if (m_spCameraComponent)			m_spCameraComponent.reset();
	if (m_spCollisionComponent)			m_spCollisionComponent.reset();
	if (m_spInputComponent)				m_spInputComponent.reset();
	if (m_spDrawModelComponent)			m_spDrawModelComponent.reset();
	if (m_spCollisionModelComponent)	m_spCollisionModelComponent.reset();
}

void GameObject::TransformSetting(const json11::Json& json_object)
{
	if (json_object.is_null()) { return; }

	// 座標
	KdMatrix transMat = {};
	const std::vector<json11::Json>& rPos = json_object["Position"].array_items();
	if (rPos.size() == 3)
	{
		transMat.CreateTranslation((float)rPos[0].number_value(), (float)rPos[1].number_value(), (float)rPos[2].number_value());
	}

	// 回転
	KdMatrix rotateMat = {};
	const std::vector<json11::Json>& rRot = json_object["Rotation"].array_items();
	if (rRot.size() == 3)
	{
		rotateMat.CreateRotationX(static_cast<float>(rRot[0].number_value() * KdToRadians));
		rotateMat.RotateY(static_cast<float>(rRot[1].number_value() * KdToRadians));
		rotateMat.RotateZ(static_cast<float>(rRot[2].number_value() * KdToRadians));
	}

	// 拡大縮小
	KdMatrix scaleMat = {};
	const std::vector<json11::Json>& rScale = json_object["Scaling"].array_items();
	if (rScale.size() == 3)
	{
		scaleMat.CreateScalling((float)rScale[0].number_value(), (float)rScale[1].number_value(), (float)rScale[2].number_value());
	}

	// 行列作成
	m_worldMatrix = scaleMat * rotateMat * transMat;
}

void GameObject::ParameterSetting(const json11::Json& json_object)
{
	if (json_object.is_null()) { return; }

	// クラスの名前
	if (!json_object["ClassName"].is_null()) m_className = json_object["ClassName"].string_value();
	// 名前
	if (!json_object["Name"].is_null()) m_name = json_object["Name"].string_value();
	// 識別タグ
	if (!json_object["Tag"].is_null()) m_tag = json_object["Tag"].int_value();
	// Prefab
	if (!json_object["Prefab"].is_null()) m_prefabFilepath = json_object["Prefab"].string_value();
	// 当たり判定に使用する半径
	if (!json_object["ColRadius"].is_null()) m_colRadius = (float)json_object["ColRadius"].number_value();

	// 描画用モデル
	if (!json_object["ModelFilepath"].is_null()) {
		m_modelName = json_object["ModelFilepath"].string_value();
		m_spDrawModelComponent->SetModel(KdResFac.GetModel(json_object["ModelFilepath"].string_value()));
	}
	// 衝突判定用モデル
	if (!json_object["ColModelFilepath"].is_null()) {
		m_spCollisionModelComponent->SetModel(KdResFac.GetModel(json_object["ColModelFilepath"].string_value()));
	}
	else {
		// 見つからなければ描画用モデルを使用
		if (!json_object["ModelFilepath"].is_null()) {
			m_modelName = json_object["ModelFilepath"].string_value();
			m_spCollisionModelComponent->SetModel(KdResFac.GetModel(json_object["ModelFilepath"].string_value()));
		}
	}
}