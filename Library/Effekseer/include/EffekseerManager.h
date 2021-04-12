//-----------------------------------------------------------------------------
// File: EffekseerManager.h
//
// Edit: 前島先生
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
class EffekseerEffectInstance;

//-----------------------------------------------------------------------------
// Name: class EffekseerManager
// Desc: Effekseerを管理するクラス Singletonパターン
//-----------------------------------------------------------------------------
class EffekseerManager
{
public:
	// インスタンスの所得
	static EffekseerManager& GetInstance()
	{
		static EffekseerManager instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 解放
	void Finalize();

	// 更新
	void Update();

	// 再生
	bool Play(const std::u16string& filePath,KdVector3& position);

	// エフェクト読み込み
	std::shared_ptr<EffekseerEffectInstance> LoadEffect(const std::u16string& filepath);

	// エフェクト検索
	std::shared_ptr<EffekseerEffectInstance> FindEffect(const std::u16string& filepath);

	// 全体の再生を止める
	void StopAll();

	// カメラ行列設定
	void SetCameraMatrix(KdMatrix& viewMatrix, KdMatrix& projMatrix);

	// Effekseer本体のManager取得
	Effekseer::Manager* GetManager() { return m_pEffekseerManager; }

private:
	//--------------------------------------------------
	// ローカルデータ
	//--------------------------------------------------
	Effekseer::Manager*													m_pEffekseerManager;		// Effeseer本体
	EffekseerRendererDX11::Renderer*									m_pEffekseerRenderer;		// Direct11用Renderer
	std::map<std::u16string, std::shared_ptr<EffekseerEffectInstance>>	m_instanceMap;				// 各エフェクトのインスタンス管理
	const int															m_maxDrawSpriteCount = 8000;// 最大描画数

private:
	// コンストラクタ
	EffekseerManager() {}
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define EFFEKSEER EffekseerManager::GetInstance()

//-----------------------------------------------------------------------------
// Name: class EffekseerEffect
// Desc: エフェクトデータクラス
//-----------------------------------------------------------------------------
class EffekseerEffect
{
public:
	EffekseerEffect() {}
	~EffekseerEffect() { ES_SAFE_RELEASE(m_pEffect); }

	// 読み込み
	bool Load(const std::u16string& filepath);

	// エフェクトデータ取得
	Effekseer::Effect* Get() { return m_pEffect; }

private:
	Effekseer::Effect*	m_pEffect;	// Effekseerのエフェクトデータ

	// コピー禁止用:単一のデータはコピーできない
	EffekseerEffect(const EffekseerEffect& src) = delete;
	void operator=(const EffekseerEffect& src) = delete;
};

//-----------------------------------------------------------------------------
// Name: class EffekseerEffectInstance
// Desc: エフェクト再生用のインスタンスクラス
//-----------------------------------------------------------------------------
class EffekseerEffectInstance : public std::enable_shared_from_this<EffekseerEffectInstance>
{
public:
	EffekseerEffectInstance() {}
	~EffekseerEffectInstance() {}

	// 初期化
	void Initialize(const std::shared_ptr<EffekseerEffect>& effectData);

	// エフェクトを再生
	void Play(const KdVector3& position);

	// 再生中のエフェクトを移動
	void Move(KdVector3& addPosition);

	// 再生停止
	void Stop();

	// データ取得
	std::shared_ptr<EffekseerEffect> GetData() { return m_effectData; }

	void SetHandle(Effekseer::Handle handle) { m_handle = handle; }
	inline Effekseer::Handle GetHandle() { return m_handle; }

private:
	Effekseer::Handle					m_handle;		// エフェクトのハンドル(index)
	std::shared_ptr<EffekseerEffect>	m_effectData;	// Effekseerのエフェクトデータ

	// コピー禁止用:単一のデータはコピーできない
	EffekseerEffectInstance(const EffekseerEffectInstance& src) = delete;
	void operator=(const EffekseerEffectInstance& src) = delete;
};