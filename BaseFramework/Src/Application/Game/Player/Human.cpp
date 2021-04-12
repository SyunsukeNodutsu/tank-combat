//-----------------------------------------------------------------------------
// File: Human.cpp
// 
// Edit: 2021/03/23 野筒隼輔
//-----------------------------------------------------------------------------
#include "Human.h"
#include "Application/Component/ModelComponent.h"

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
Human::Human()
	: m_animator()
	, m_animationSpeed(1.0f)
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void Human::Deserialize(const json11::Json& json_object)
{
	// 基底クラスの初期化(共通の初期化)
	GameObject::Deserialize(json_object);

	// アニメーション Standから
	SetAnimation("Armature");
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void Human::Update()
{
	if (m_spDrawModelComponent == nullptr) { return; }

	// アニメーションの更新
	m_animator.AdvanceTime(m_spDrawModelComponent->GetChangeableNodes(), m_animationSpeed);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void Human::SetAnimation(const char* name)
{
	if (m_spDrawModelComponent == nullptr) { return; }

	std::shared_ptr<KdAnimationData> apAnimData = m_spDrawModelComponent->GetAnimation(name);
	if (apAnimData) { m_animator.SetAnimation(apAnimData); }
}