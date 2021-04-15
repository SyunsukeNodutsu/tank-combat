//-----------------------------------------------------------------------------
// File: CameraManager.cpp
//
// Edit: 2021/03/28 野筒隼輔
//-----------------------------------------------------------------------------
#include "CameraManager.h"
#include "Application/Game/Other/AnimationCamera.h"
#include "Application/Game/Other/Radio.h"
#include "Application/Component/CameraComponent.h"
#include "Application/Manager/ObjectManager.h"

//-----------------------------------------------------------------------------
// Name: CameraManager()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
CameraManager::CameraManager()
	: m_wpTargetCamera()
	, m_spStart(nullptr)
	, m_animationSpeed(0.04f)
	, m_isAnimationEnable(false)
	, m_isAnimation(false)
{
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void CameraManager::Update()
{
	static int count = 0;
	constexpr int count_max = 60;

	// アニメーション中
	if (m_isAnimation) {
		count++;
		if (count >= count_max) {
			// 指定のフレームでアニメーションを終了 -> 元のカメラに戻る
			SetTargetCamera(m_spStart->GetCameraComponent());
			m_isAnimation = false;
			count = 0;
			// 無線表示
			RADIO.Emit(WIRELESS_TYPE::eDefeat, OWNER_TYPE::ePlayer, true);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Reset()
// Desc: リセット
//-----------------------------------------------------------------------------
void CameraManager::Reset()
{
	m_wpTargetCamera.reset();
}

//-----------------------------------------------------------------------------
// Name: SetToShader()
// Desc: Shaderに転送
//-----------------------------------------------------------------------------
void CameraManager::SetToShader()
{
	if (m_wpTargetCamera.lock() == nullptr) { return; }
	m_wpTargetCamera.lock()->SetToShader();
}

//-----------------------------------------------------------------------------
// Name: StartAnimation()
// Desc: 撃破時のアニメーションを開始
//-----------------------------------------------------------------------------
void CameraManager::StartAnimation(std::shared_ptr<GameObject> object, std::string name)
{
	if (!m_isAnimationEnable) { return; }

	// 既にアニメーション中かどうか確認
	auto animCam = OBJ_MAGER.SearchObject("AnimationCamera");
	if (animCam) { return; }

	// 開始GameObject
	m_spStart = object;

	// 切り替えたFlgON
	m_isAnimation = true;

	// 新しいターゲットのキャラクタを検索
	auto target = OBJ_MAGER.SearchObject(name);
	if (!target) {
		assert(0 && "[ChangeCameraTarget] : 切り替え先のGameObjectが存在しません");
		return;
	}

	// 補完用のキャラクタを作成
	auto newAnimCam = std::make_shared<AnimationCamera>();
	if (newAnimCam) {
		// 情報の設定
		newAnimCam->SetInfo(m_spStart, target, m_animationSpeed);
		// 補完用キャラクタをターゲットカメラに設定
		SetTargetCamera(newAnimCam->GetCameraComponent());
		// 補完用キャラクタを管理Objectリストに追加
		OBJ_MAGER.AddObject(newAnimCam);
	}
}