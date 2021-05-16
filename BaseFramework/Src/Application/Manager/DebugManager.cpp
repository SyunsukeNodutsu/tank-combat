//-----------------------------------------------------------------------------
// File: DebugManager.cpp
//
// Edit: 2021/03/29 野筒隼輔
//-----------------------------------------------------------------------------
#include "DebugManager.h"
#include "ObjectManager.h"
#include "Application/main.h"
#include "Application/Game/GameObject.h"
#include "Application/Game/Scene/Scene.h"
#include "Application/Game/Scene/Pause.h"

#include "Application/Manager/CameraManager.h"

#include "Application/Component/CameraComponent.h"
#include "Application/Component/CollisionComponent.h"

//-----------------------------------------------------------------------------
// Name: DebugManager()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
DebugManager::DebugManager()
	: m_spLogWindow(nullptr)
	, m_wpSelectObject()
	, m_isNoDamage(true)
	, m_isCollModelShow(false)
	, m_isOBBShow(false)
	, m_isPointListShow(false)
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void DebugManager::Initialize()
{
	// log window 生成
	m_spLogWindow = std::make_shared<ImGuiLogWindow>();
	if (m_spLogWindow == nullptr) {
		assert(0 && "LogWindow生成失敗");
	}
}

//-----------------------------------------------------------------------------
// Name: Serialize()
// Desc: object -> データ
//-----------------------------------------------------------------------------
void DebugManager::Serialize(const std::string a_path)
{
	// 保存したい値を集約
	json11::Json::object serial = {};

	json11::Json::array  objectArray = {};

	for (auto& spObject : OBJ_MAGER.GetObjectList())
	{
		if (spObject == nullptr) continue;

		json11::Json::object object = {};

		// クラスの名前
		object["ClassName"] = spObject->GetClasName();

		// モデルの名前
		object["ModelFileName"] = spObject->GetModelName();

		// Prefab
		object["Prefab"] = spObject->GetPrefabFilepath();

		// 名前
		object["Name"] = spObject->GetName();

		// 座標
		json11::Json::array position(3);
		position[0] = spObject->GetMatrix().GetTranslation().x;
		position[1] = spObject->GetMatrix().GetTranslation().y;
		position[2] = spObject->GetMatrix().GetTranslation().z;
		object["Position"] = position;

		// 回転率
		json11::Json::array rotation(3);
		rotation[0] = spObject->GetMatrix().GetAngle().x * KdToDegrees;
		rotation[1] = spObject->GetMatrix().GetAngle().y * KdToDegrees;
		rotation[2] = spObject->GetMatrix().GetAngle().z * KdToDegrees;
		object["Rotation"] = rotation;

		// 拡大率
		json11::Json::array scaling(3);
		scaling[0] = spObject->GetMatrix().GetScale().x;
		scaling[1] = spObject->GetMatrix().GetScale().y;
		scaling[2] = spObject->GetMatrix().GetScale().z;
		object["Scaling"] = scaling;

		// タグ
		object["Tag"] = static_cast<int>(spObject->GetTag());

		// 配列に要素を追加
		objectArray.push_back(object);
	}

	// オブジェクトを格納した配列を保存
	serial["GameObjects"] = objectArray;

	// Jsonファイル所得 ※初回は作成
	std::ofstream ofs(a_path);
	if (!ofs) return;

	// オブジェクトをJsonへ変換
	json11::Json json = serial;
	// Jsonのフォーマットにあった書式に変換しながら文字列に
	std::string strJson = json.dumpDebug();

	// 指定されたJsonファイルへ文字列を書き込み
	ofs.write(strJson.c_str(), strJson.size());
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void DebugManager::Update()
{
	if (!SCENE.IsEdiorMode()) { return; }
	if (PAUSE.IsEnable()) { return; }

	static bool isPush = true;
	if (GetAsyncKeyState('2') & 0x8000)
	{
		if (!isPush)
		{
			isPush = !isPush;

			// マウスの情報所得
			POINT mousePos = {};
			GetCursorPos(&mousePos);
			ScreenToClient(APP.m_window.GetWndHandle(), &mousePos);

			// カメラ確認
			auto camera = CAMERA_MAGER.GetTargetCamera().lock();
			if (camera == nullptr) return;

			// レイ情報設定
			RayInfo rayInfo = {};
			camera->CreateRayInfoFromPlanePos(rayInfo, Math::Vector2(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)));

			// 結果格納用
			KdRayResult finalResult = {};
			std::shared_ptr<GameObject> targetObj = nullptr;

			// GameObjectList
			for (auto& spObject : OBJ_MAGER.GetObjectList())
			{
				// 判定
				KdRayResult tmpResult = {};
				if (!spObject->GetCollisionComponent()->HitCheckByRay(rayInfo, tmpResult)) continue;

				// より近いObjectが無いか確認
				if (tmpResult.m_distance < finalResult.m_distance)
				{
					finalResult = tmpResult;
					targetObj = spObject;
				}
			}

			// なにかのGameObjetに当たった場合、選択Objectに指定
			if (targetObj) { m_wpSelectObject = targetObj; }
		}
	}
	else if (isPush) { isPush = !isPush; }
}

//-----------------------------------------------------------------------------
// Name: ImGuiUpdate()
// Desc: ImGui更新
//-----------------------------------------------------------------------------
void DebugManager::ImGuiUpdate()
{
	//--------------------------------------------------
	// Flg Setting
	//--------------------------------------------------
	if (ImGui::TreeNodeEx("Flg Setting", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("NoDamage",			&m_isNoDamage);			// 無敵状態
		ImGui::Checkbox("CollModelShow",	&m_isCollModelShow);	// コリジョンモデル表示
		ImGui::Checkbox("OBBShow",			&m_isOBBShow);			// バウンディングボックス表示
		ImGui::Checkbox("PointListShow",	&m_isPointListShow);	// 経由地点表示

		ImGui::TreePop();
	}

	//--------------------------------------------------
	// ObjectList
	//--------------------------------------------------
	if (ImGui::TreeNodeEx("ObjectList", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto&& object : OBJ_MAGER.GetObjectList()) {
			ImGui::PushID(object.get());

			// 選択したObject
			bool selected = (object == m_wpSelectObject.lock());
			if (ImGui::Selectable(object->GetName(), selected)) {
				m_wpSelectObject = object;
			}

			ImGui::PopID();
		}

		ImGui::TreePop();
	}

	//--------------------------------------------------
	// PrefabFactory
	//--------------------------------------------------
	if (ImGui::TreeNodeEx("PrefabFactory"))
	{
		static char jsonpath[128] = "";// 所得後、格納
		static std::string string = "Data/Text/Object/";// パスを所得する階層

		ImGui::InputText("path", jsonpath, IM_ARRAYSIZE(jsonpath));

		// objectを生成
		if (ImGui::Button("create object")) { CreateObject(); }
		// jsonパスを所得
		if (ImGui::Button("get jsonpath")) { GetJsonpath(jsonpath); }
		// シーンのobject一覧をjsonに保存
		if (ImGui::Button("save json")) { SaveJson(); }

		ImGui::TreePop();
	}

	//--------------------------------------------------
	// Inspector
	//--------------------------------------------------
	if (ImGui::TreeNodeEx("Inspector"))
	{
		if (m_wpSelectObject.lock()) { m_wpSelectObject.lock()->ImGuiUpdate(); }
		ImGui::TreePop();
	}
}

//-----------------------------------------------------------------------------
// Name: CreateObject()
// Desc: object生成
//-----------------------------------------------------------------------------
void DebugManager::CreateObject()
{

}

//-----------------------------------------------------------------------------
// Name: GetJsonpath()
// Desc: jsonpathを所得
//-----------------------------------------------------------------------------
void DebugManager::GetJsonpath(const std::string& result)
{

}

//-----------------------------------------------------------------------------
// Name: SaveJson()
// Desc: シーンのobject一覧をjsonに保存
//-----------------------------------------------------------------------------
void DebugManager::SaveJson()
{

}