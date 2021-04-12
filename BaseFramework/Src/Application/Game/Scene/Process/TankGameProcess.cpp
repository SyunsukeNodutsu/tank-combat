//-----------------------------------------------------------------------------
// File: TankGameProcess.cpp
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#include "TankGameProcess.h"
#include "Application/Game/Other/Radio.h"
#include "Application/Game/Scene/Scene.h"
#include "Application/Game/Scene/Pause.h"
#include "Application/Game/Weapon/Tank.h"
#include "Application/Manager/ClearManager.h"
#include "Application/Manager/ObjectManager.h"

//-----------------------------------------------------------------------------
// Name: TankGameProcess()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
TankGameProcess::TankGameProcess()
	: m_onceEmitRadio(false)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void TankGameProcess::Deserialize(const json11::Json& json_object)
{
	GameObject::Deserialize(json_object);
	SCENE.SetCursorShow(false);
	PAUSE.SetGamePlaying(true);
	// BGM再生
	AUDIO.Play("Data/Audio/BGM/Germany.wav", 0.6f, true);
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void TankGameProcess::Update()
{
	// 初回の無線発生
	if (!m_onceEmitRadio) { OnceEmitRadio(); }
}

//-----------------------------------------------------------------------------
// Name: OnceEmitRadio()
// Desc: ステージの開始時に一度のみ発生する無線
//-----------------------------------------------------------------------------
void TankGameProcess::OnceEmitRadio()
{
	constexpr std::uint8_t count_max = 30;

	static std::uint8_t count = 0;
	count++;
	if (count >= count_max) {
		count = 0;
		m_onceEmitRadio = true;
		RADIO.Emit(WIRELESS_TYPE::eStartAttack, OWNER_TYPE::ePlayer, true, 120);

		// 味方戦車のコンテナを結果用に設定する ※修正.関数分け
		std::list<std::shared_ptr<Tank>> friendlyTanklist = {};
		for (auto& tank : OBJ_MAGER.GetTankList()) {
			if (tank->GetTag() & OBJECT_TAG::TAG_Enemy) { continue; }
			friendlyTanklist.push_back(tank);
		}
		CLEAR_MAGER.SetFriendlyTankList(friendlyTanklist);
	}
}