//-----------------------------------------------------------------------------
// File: Audio.h
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class SoundEffect
// Desc: サウンドの元データクラス
//-----------------------------------------------------------------------------
class SoundEffect
{
public:

	// コンストラクタ
	SoundEffect() {}
	// デストラクタ
	~SoundEffect() { m_soundEffect = nullptr; }
	// WAVE読み込み
	bool Load(const std::string& aFilepath);
	// インスタンス作成
	std::unique_ptr<DirectX::SoundEffectInstance> CreateInstance(DirectX::SOUND_EFFECT_INSTANCE_FLAGS aFlags);

private:
	std::unique_ptr<DirectX::SoundEffect>	m_soundEffect; // サウンドエフェクト

	// コピー禁止用 ※単一のデータはコピーできない
	SoundEffect(const SoundEffect& src) = delete;
	void operator=(const SoundEffect& src) = delete;
};

//-----------------------------------------------------------------------------
// Name: class SoundInstance
// Desc: サウンドの再生インスタンスクラス(鳴らす度に生成)
//       DirectX::SoundEffectInstanceをshared_ptrで使用可能に(ラップ)
//-----------------------------------------------------------------------------
class SoundInstance : public std::enable_shared_from_this<SoundInstance>
{
public:

	// コンストラクタ
	SoundInstance();
	// 初期化
	virtual void Initialize(const std::shared_ptr<SoundEffect>& a_soundEffect);
	// 再生
	virtual void Play(const bool a_isLoop = false);
	// 停止
	virtual void Stop() { m_instance->Stop(); }
	// 徐々に音量を下げる
	virtual void SetFadeOut() { m_isFadeOut = true; }
	//
	virtual void UpdateFadeOut();

	//--------------------------------------------------
	// 設定.所得
	//--------------------------------------------------
	// 音量設定
	void SetVolume(const float volume);
	// 再生中か所得
	bool IsPlaying();

protected:
	std::unique_ptr<DirectX::SoundEffectInstance>	m_instance;	// サウンドの再生インスタンス
	std::shared_ptr<SoundEffect>					m_soundData;// 再生サウンドの元データ
	float											m_volume;	// 音量
	bool											m_isFadeOut;// フェードアウトするかどうか
	bool											m_isDone;	// 初期化が完了しているか

	// コピー禁止用
	SoundInstance(const SoundInstance& src) = delete;
	void operator=(const SoundInstance& src) = delete;
};

//-----------------------------------------------------------------------------
// Name: class SoundInstance3D
// Desc: 3D用インスタンス
//-----------------------------------------------------------------------------
class SoundInstance3D : public SoundInstance
{
public:

	// コンストラクタ
	SoundInstance3D();
	// 初期化
	void Initialize(const std::shared_ptr<SoundEffect>& a_soundEffect) override;
	// 再生
	void Play(const bool a_isLoop = false) override;
	// 発生源の座標設定
	void SetPosition(const Math::Vector3& a_pos);

private:
	DirectX::AudioEmitter m_emitter; // エミッター(主にサウンドソースの定義)

	// コピー禁止用
	SoundInstance3D(const SoundInstance3D& src) = delete;
	void operator=(const SoundInstance3D& src) = delete;
};

//-----------------------------------------------------------------------------
// Name: class AudioManager
// Desc: 音管理クラス
//-----------------------------------------------------------------------------
class AudioManager
{
public:

	// 初期化
	bool Initialize();
	// 解放
	void Release();
	// 更新
	void Update(const Math::Vector3& a_pos, const Math::Vector3& a_dir);

	//--------------------------------------------------
	// サウンド操作
	//--------------------------------------------------
	// サウンド再生
	bool Play(const std::string& filepath, const float volume = 1.0f, const bool loop = false);
	// 3Dサウンド再生 ※ループは考慮しない
	bool Play3D(const std::string& filepath, const Math::Vector3& pos, const float volume = 1.0f);
	// すべてのサウンド停止
	void AllStop();
	// すべてのサウンドを徐々に音量を下げる
	void AllFadeOut();

	//--------------------------------------------------
	// サウンドのインスタンスを直接所得
	//--------------------------------------------------
	// 通常サウンド
	std::shared_ptr<SoundInstance> GetSoundInstance(const std::string& filepath);
	// 3Dサウンド
	std::shared_ptr<SoundInstance3D> GetSoundInstance3D(const std::string& filepath, const Math::Vector3& pos = Math::Vector3(0.0f, 0.0f, 0.0f));

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// 再生リストに登録
	void AddplayList(const std::shared_ptr<SoundInstance>& sound);
	// ユーザー設定の音量設定
	void SetUserSettingVolume(const float volume);

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// インスタンスの所得
	static AudioManager& GetInstance() { static AudioManager instance; return instance; }
	// AaudioEngine所得
	std::unique_ptr<DirectX::AudioEngine>& GetAudioEngine() { return m_pAudioEngine; }
	// AudioListener所得
	DirectX::AudioListener GetListener() { return m_listener; }
	// ユーザー設定の音量所得
	const float GetUserSettingVolume() const { return m_userVolume; }
	// 現在再生中のサウンド数取得
	const int GetPlayListSize() const { return static_cast<int>(m_playList.size()); }

private:
	std::unique_ptr<DirectX::AudioEngine>			m_pAudioEngine;	// オーディオエンジン ※DirectXのサウンド全体の管理
	std::map<UINT, std::shared_ptr<SoundInstance>>	m_playList;		// 再生中のサウンドリスト ※fastにはインスタンスのアドレスを設定
	DirectX::AudioListener							m_listener;		// 3Dサウンド用リスナー
	float											m_userVolume;	// ユーザー設定の音量 ※マスター音量に設定
	float											m_peakLevels[2];// ピークメータ ※瞬間最大値
	float											m_RMSLevels[2];	// RMSメータ ※平均値

private:
	AudioManager();
	void UpdateVolumeMeter();
};

//--------------------------------------------------
// Name: enum SOUND_TAG
// Desc: 音を分類 オプションで変更するため
//--------------------------------------------------
enum SOUND_TAG
{
	eSE			= 0,		// 効果音
	eBGM		= 1,		// BGM("バックグラウンドミュージック"らしいっすよ)
	eAmbient	= 1 << 1,	// 環境音
	eVoice		= 1 << 2,	// 声
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define AUDIO AudioManager::GetInstance()