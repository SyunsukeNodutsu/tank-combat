//-----------------------------------------------------------------------------
// File: KdShaderManager.h
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "EffectShader/KdEffectShader.h"
#include "GenerateShadowMapShader/KdGenerateShadowMapShader.h"
#include "ModelShader/KdModelShader.h"
#include "PostProcessShader/KdPostProcessShader.h"
#include "SpriteShader/KdSpriteShader.h"
#include "StandardShader/KdStandardShader.h"

//-----------------------------------------------------------------------------
// Name: class KdShaderManager
// Desc: シェーダ関係をまとめたクラス
//-----------------------------------------------------------------------------
class KdShaderManager
{
public:

	// 初期化
	void Init();

	// 解放
	void Release();

	//--------------------------------------------------
	// 所得.設定
	//--------------------------------------------------
	
	// インスタンスの所得
	static KdShaderManager& GetInstance() { static KdShaderManager instance; return instance; }

	// 点光を追加
	void AddPointLight(const KdVector3& pos, float radius, const KdVector3& color);

	// 点光をリセット
	void ResetPointLight();

public:
	// シェーダ
	KdStandardShader			m_standardShader;				// 3Dモデル描画シェーダ
	KdEffectShader				m_effectShader;					// エフェクト描画シェーダ
	KdSpriteShader				m_spriteShader;					// 2Dテクスチャ描画シェーダ
	KdModelShader				m_modelShader;					// 物理ベースによる3Dモデル描画シェーダー
	KdGenerateShadowMapShader	m_genShadowMapShader;			// シャドウマップ生成シェーダー
	KdPostProcessShader			m_postProcessShader;			// ポスト関連シェーダ

	//--------------------------------------------------
	// パイプラインステート
	//--------------------------------------------------

	// 深度ステンシル
	ID3D11DepthStencilState*	m_ds_ZEnable_ZWriteEnable;		// 奥行チェックあり(通常)
	ID3D11DepthStencilState*	m_ds_ZDisable_ZWriteDisable;	// なし
	ID3D11DepthStencilState*	m_ds_ZEnable_ZWriteDisable;

	// ラスタライズステート
	ID3D11RasterizerState*		m_rs_CullBack;
	ID3D11RasterizerState*		m_rs_CullNone;

	// ブレンドステート
	ID3D11BlendState*			m_bs_Alpha;
	ID3D11BlendState*			m_bs_Add;

	// サンプラーステート
	ID3D11SamplerState*			m_ss_Anisotropic_Wrap;
	ID3D11SamplerState*			m_ss_Anisotropic_Clamp;
	ID3D11SamplerState*			m_ss_Linear_Clamp;
	ID3D11SamplerState*			m_ss_ShadowMapComparison;

	//--------------------------------------------------
	// カメラ関係
	//--------------------------------------------------

	// カメラ定数バッファ用構造体
	struct cbCamera
	{
		Math::Matrix		mV;			// ビュー行列
		Math::Matrix		mP;			// 射影行列
		Math::Vector3		CamPos;		// カメラワールド座標

		int					DistanceFogEnable	= 1;			// 距離フォグ有効
		Math::Vector3		DistanceFogColor	= { 1, 1, 1 };	// 距離フォグ色
		float				DistanceFogDensity	= 0.001f;		// 距離フォグ減衰率
	};

	// カメラ定数バッファ
	KdConstantBuffer<cbCamera>	m_cb7_Camera;

	//--------------------------------------------------
	// ライト関係
	//--------------------------------------------------

	// ライト定数バッファ用構造体
	struct cbLight
	{
		// 環境光
		Math::Vector3		AmbientLight = { 0.3f, 0.3f, 0.3f };
		float				tmp;

		// 平行光
		Math::Vector3		DL_Dir = {1.0f, -1.0f, -0.5f};	// 光の方向
		float				tmp2;
		Math::Vector3		DL_Color = {6.0f, 6.0f, 6.0f};	// 光の色
		float				tmp3;

		KdMatrix            DL_mLightVP; // ライトカメラのビュー行列*射影行列

		//-----------------
		// 点光
		//-----------------
		// 使用数
		int					PL_Cnt = 0;
		float				tmp4[3];

		// データ
		struct PointLight
		{
			KdVector3		Color;					// 色
			float			Radius;					// 半径
			KdVector3		Pos;					// 座標
			float			tmp;
		};
		std::array<PointLight, 100> PL;
	};

	// ライト定数バッファ
	KdConstantBuffer<cbLight>		m_cb8_Light;

private:
	// コンストラクタ
	KdShaderManager();
};

//-----------------------------------------------------------------------------
// define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define SHADER KdShaderManager::GetInstance()