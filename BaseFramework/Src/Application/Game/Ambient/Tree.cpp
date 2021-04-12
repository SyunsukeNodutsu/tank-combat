//-----------------------------------------------------------------------------
// File: Tree.h
//
// Edit: 2021/03/18 野筒隼輔
//-----------------------------------------------------------------------------
#include "Tree.h"
#include "Application/Component/ModelComponent.h"

//-----------------------------------------------------------------------------
// Name: Tree()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
Tree::Tree()
	: m_spTreeGrass(nullptr)
	, m_treeGrassLocalMatrix()
	, m_isCollapse(false)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void Tree::Deserialize(const json11::Json& json_object)
{
	GameObject::Deserialize(json_object);

	//--------------------------------------------------
	// ランダムで回転と拡大縮小
	//--------------------------------------------------
	{
		// 擬似乱数生成器を、ハードウェア乱数をシードにして初期化
		static std::random_device seed_gen;
		std::mt19937 engine(seed_gen());
		// 一様実数分布
		std::uniform_real_distribution<float> dist_rot(-180.0f, 180.0f);// -180～180
		std::uniform_real_distribution<float> dist_size(0.9f, 1.2f);// 0.9～1.2

		// 回転
		KdMatrix rotMat = {};
		rotMat.CreateRotationX(0.0f * KdToRadians);
		rotMat.RotateY(dist_rot(engine) * KdToRadians);
		m_worldMatrix = rotMat * m_worldMatrix;
		// 拡大縮小
		KdMatrix scaleMat = {};
		scaleMat.CreateScalling(KdVector3(dist_size(engine), dist_size(engine), dist_size(engine)));
		m_worldMatrix = scaleMat * m_worldMatrix;
	}

	//--------------------------------------------------
	// 親子構造の葉っぱ 生成
	//--------------------------------------------------
	m_spTreeGrass = std::make_shared<GameObject>();
	if (m_spTreeGrass) {
		m_spTreeGrass->Deserialize(json_object["TreeGrass"].object_items());
		// 揺れON
	//	m_spTreeGrass->GetModelComponent()->SetWave(true);
		// 本体からのずれを設定
		m_treeGrassLocalMatrix.CreateTranslation(0.0f, 0.0f, 0.0f);
	}
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void Tree::Update()
{
	// 葉っぱ更新
	if (m_spTreeGrass) { m_spTreeGrass->SetMatrix(m_treeGrassLocalMatrix * m_worldMatrix); }

	// 倒壊フラグ確認
	if (!m_isCollapse) { return; }

	// 下に落ちていく
	m_worldMatrix.Move(KdVector3(0.0f, 0.0f, 1.0f));
}

//-----------------------------------------------------------------------------
// Name: DrawEffect()
// Desc: Effect描画
//-----------------------------------------------------------------------------
void Tree::DrawEffect()
{
	// 葉っぱ
	if (m_spTreeGrass) { m_spTreeGrass->GetModelComponent()->DrawEffect(); }
}

//-----------------------------------------------------------------------------
// Name: DrawShadowMap()
// Desc: シャドウマップ生成描画
//-----------------------------------------------------------------------------
void Tree::DrawShadowMap()
{
	// 幹のシャドウ描画
	GameObject::DrawShadowMap();
	// 葉っぱ
	if (m_spTreeGrass) { m_spTreeGrass->GetModelComponent()->DrawShadowMap(); }
}