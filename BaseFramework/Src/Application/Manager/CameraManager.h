//-----------------------------------------------------------------------------
// File: CameraManager.h
//
// Edit: 2021/03/28 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
class GameObject;
class CameraComponent;

//-----------------------------------------------------------------------------
// Name: class CameraManager
// Desc: 使用するカメラを管理 撃破時のカメラアニメーションも
//-----------------------------------------------------------------------------
class CameraManager
{
public:

	// 更新 カメラアニメーションの後にカメラを元に戻す
	void Update();
	// リセット
	void Reset();
	// シェーダへ転送
	void SetToShader();

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// インスタンスの所得
	static CameraManager& GetInstance() { static CameraManager instance; return instance; }
	// ターゲットカメラを所得
	const std::weak_ptr<CameraComponent> GetTargetCamera() const { return m_wpTargetCamera; }
	// 撃破後のカメラアニメーションを行うかどうか
	const bool IsAnimationEnable() const { return m_isAnimationEnable; }
	// 撃破時のアニメーション中かどうか
	const bool IsAnimation() const { return m_isAnimation; }

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// 撃破アニメーション開始
	void StartAnimation(std::shared_ptr<GameObject> object, std::string name);
	// 引数のカメラをターゲットカメラに設定
	inline void SetTargetCamera(std::shared_ptr<CameraComponent> camera) { m_wpTargetCamera = camera; }
	// 撃破後のアニメーションON.OFFを設定
	inline void SetCameraAnimation(const bool enable) { m_isAnimationEnable = enable; }

private:
	std::weak_ptr<CameraComponent>	m_wpTargetCamera;		// ターゲットのカメラ
	std::shared_ptr<GameObject>		m_spStart;				// 撃破アニメーション時のカメラ初期GameObject
	float							m_animationSpeed;		// 撃破時のアニメーションスピード
	bool							m_isAnimationEnable;	// 撃破時のアニメーションを行うかどうか
	bool							m_isAnimation;			// 撃破時のアニメーション中かどうか

private:
	CameraManager();
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define CAMERA_MAGER CameraManager::GetInstance()