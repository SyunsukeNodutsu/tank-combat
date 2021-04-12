//-----------------------------------------------------------------------------
// File: ObjectManager.cpp
//
// Edit: 2021/02/11 野筒隼輔
//-----------------------------------------------------------------------------
#include "ObjectManager.h"
#include "DebugManager.h"
#include "DrawManager.h"

#include "Application/Game/Weapon/Tank.h"
#include "Application/Game/Scene/InterruptCheck.h"

//-----------------------------------------------------------------------------
// Name: ObjectManager()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
ObjectManager::ObjectManager()
	: m_spObjectList()
	, m_spTankList()
{
}

//-----------------------------------------------------------------------------
// Name: Reset()
// Desc: リセット
//-----------------------------------------------------------------------------
void ObjectManager::Reset()
{
	m_spObjectList.clear();
	m_spTankList.clear();
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void ObjectManager::Update()
{
	if (IRQ.IsEnable()) { return; }

	for (auto pObjectItr = m_spObjectList.begin(); pObjectItr != m_spObjectList.end();) {
		// 有効確認
		if (!(*pObjectItr)->IsEnable()) {
			// 除外
			pObjectItr = m_spObjectList.erase(pObjectItr);
			continue;
		}
		// デバッグ選択されたObject
		if ((*pObjectItr) == DEBUG_MAGER.GetSelectObject().lock()) {
			// 点滅
			static Math::Color color = kWhiteColor;
			static int cnt = 0;
			cnt++;
			color = (cnt >= 30) ? kRedColor : kWhiteColor;
			if (cnt >= 60) { cnt = 0; }
			// 球描画
			DRAW_MAGER.AddDebugSphereLine((*pObjectItr)->GetMatrix().GetTranslation(), (*pObjectItr)->GetMatrix().GetScale().x, color);
		}
		else {
			// 更新
			(*pObjectItr)->Update();
		}
		++pObjectItr;
	}
}

//-----------------------------------------------------------------------------
// Name: AddTank()
// Desc: 戦車のコンテナに追加
//-----------------------------------------------------------------------------
void ObjectManager::AddObject(const std::shared_ptr<GameObject> object)
{
	// Objectコンテナに追加
	m_spObjectList.push_back(object);

	// Tankコンテナに追加
	if (object->GetTag() & OBJECT_TAG::TAG_Tank) {
		std::shared_ptr<Tank> tank = std::dynamic_pointer_cast<Tank>(object);
		if (tank == nullptr) { return; }
		m_spTankList.push_back(tank);
	}
}

//-----------------------------------------------------------------------------
// Name: SearchObject()
// Desc: Objectを名前検索
//-----------------------------------------------------------------------------
const std::shared_ptr<GameObject> ObjectManager::SearchObject(const std::string& name)
{
	for (auto&& object : m_spObjectList) {
		if (object->GetName() == name) { return object; }
	}
	return nullptr;// error
}