//-----------------------------------------------------------------------------
// File: DrawManager.h
//
// Edit: 2021/02/11 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class DrawManager
// Desc: ゲームに登場するObjectの描画を一元管理
//-----------------------------------------------------------------------------
class DrawManager
{
public:

	// 初期化
	void Initialize();
	// 描画
	void Draw();

	//--------------------------------------------------
	// デバッグ用描画
	//--------------------------------------------------
	// 直線
	void AddDebugLine(const Math::Vector3& a_pos01, const Math::Vector3& a_pos02, const Math::Color& a_color = kWhiteColor);
	// 球
	void AddDebugSphereLine(const Math::Vector3& a_pos, const float a_radius, const Math::Color& a_color = kWhiteColor);

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// ぼかしの値を設定
	inline void  SetBlurValue(Math::Vector2 a_blurValue) { m_blurValue = a_blurValue; }

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// インスタンスの所得
	static DrawManager& GetInstance() { static DrawManager instance; return instance; }

private:
	std::vector<KdEffectShader::Vertex>	m_debugLines;			// デバッグライン用の頂点情報
	std::shared_ptr<KdTexture>			m_spScreenRT;			// 加工用レンダーターゲット ※ブルームとブラー両方で使用
	std::shared_ptr<KdTexture>			m_spScreenZ;
	std::shared_ptr<KdTexture>			m_spHeightBrightTex;	// ブルーム用
	KdBlurTexture						m_blurTex;				// ブラー用
	Math::Vector2						m_blurValue;			// ぼかす値

private:
	DrawManager();
	void DrawShadowMap();
	void DrawOpaque();
	void DrawTranslucent();
	void DrawSprite();
	void DrawDebugLines();
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define DRAW_MAGER DrawManager::GetInstance()