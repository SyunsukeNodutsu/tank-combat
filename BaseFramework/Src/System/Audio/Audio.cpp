//-----------------------------------------------------------------------------
// File: Audio.cpp
//
// Edit: 2021/03/07 野筒隼輔
//-----------------------------------------------------------------------------
#include "Audio.h"

//=============================================================================
// 
// SoundEffect
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: Load()
// Desc: 読み込み
//-----------------------------------------------------------------------------
bool SoundEffect::Load(const std::string& filepath)
{
	if (AUDIO.GetAudioEngine().get() == nullptr) { return false; }

	try {
		// wstringに変換(const char* は受け取っていけない)
		std::wstring wFilepath = sjis_to_wide(filepath);
		// 読み込み
		m_soundEffect = std::make_unique<DirectX::SoundEffect>(AUDIO.GetAudioEngine().get(), wFilepath.c_str());
	}
	catch (...) {
		assert(0 && "[Load] : error load wavefile");
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: CreateInstance()
// Desc: インスタンスの生成
//-----------------------------------------------------------------------------
std::unique_ptr<DirectX::SoundEffectInstance> SoundEffect::CreateInstance(DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags)
{
	if (m_soundEffect == nullptr) { return nullptr; }
	return m_soundEffect->CreateInstance(flags);
}



//=============================================================================
// 
// SoundInstance
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: SoundInstance()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
SoundInstance::SoundInstance()
	: m_instance(nullptr)
	, m_soundData(nullptr)
	, m_volume(0.0f)
	, m_isFadeOut(false)
	, m_isDone(false)
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void SoundInstance::Initialize(const std::shared_ptr<SoundEffect>& sound_effect)
{
	if (m_isDone) { return; }// 既に初期化済み
	if (!sound_effect) { return; }

	// フラグ設定 -> インスタンス生成
	DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default;
	m_instance = (sound_effect->CreateInstance(flags));

	// サウンドの元データ設定
	m_soundData = sound_effect;

	m_isDone = true;
}

//-----------------------------------------------------------------------------
// Name: Play()
// Desc: 再生
//-----------------------------------------------------------------------------
void SoundInstance::Play(const bool loop)
{
	if (!m_instance) { return; }

	m_instance->Stop();// 事前に停止
	m_instance->Play(loop);

	// 再生中リストに登録
	AUDIO.AddplayList(shared_from_this());
}

//-----------------------------------------------------------------------------
// Name: SetVolume()
// Desc: 音量設定
//-----------------------------------------------------------------------------
void SoundInstance::SetVolume(const float volume)
{
	if (m_instance == nullptr) { return; }
	m_instance.get()->SetVolume(volume);
	m_volume = volume;
}

//-----------------------------------------------------------------------------
// Name: IsPlaying()
// Desc: 再生済みか確認
//-----------------------------------------------------------------------------
bool SoundInstance::IsPlaying()
{
	if (m_instance == nullptr) { return false; }
	return(m_instance.get()->GetState() == DirectX::SoundState::PLAYING);
}

//-----------------------------------------------------------------------------
// Name: UpdateFadeOut()
// Desc: フェード更新
//-----------------------------------------------------------------------------
void SoundInstance::UpdateFadeOut()
{
	if (!m_isFadeOut) { return; }

	// 音量が0以下になると完了
	if (m_volume <= 0) {
		m_volume = 0;
		m_instance->SetVolume(m_volume);
		m_isFadeOut = false;
		return;
	}

	// フェード処理
	constexpr float fadeSpeed = 0.006f;
	m_volume -= fadeSpeed;
	m_instance->SetVolume(m_volume);
}



//=============================================================================
// 
// SoundInstance3D
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: SoundInstance3D()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
SoundInstance3D::SoundInstance3D()
	: m_emitter()
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
void SoundInstance3D::Initialize(const std::shared_ptr<SoundEffect>& sound_effect)
{
	if (m_isDone) { return; }// 既に初期化済み
	if (!sound_effect) { return; }

	// フラグ設定 -> インスタンス生成
	DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default | DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters;
	m_instance = (sound_effect->CreateInstance(flags));

	// サウンドの元データ設定
	m_soundData = sound_effect;

	//--------------------------------------------------
	// エミッター設定
	//--------------------------------------------------
	m_emitter.InnerRadius			= 2.0f;
	m_emitter.InnerRadiusAngle		= X3DAUDIO_PI / 4.0f;
	m_emitter.pVolumeCurve			= NULL;
	m_emitter.CurveDistanceScaler	= 60.0f;// ワールド定義の単位 ※修正.定数に
	m_emitter.DopplerScaler			= 60.0f;

	m_isDone = true;
}

//-----------------------------------------------------------------------------
// Name: Play()
// Desc: 再生
//-----------------------------------------------------------------------------
void SoundInstance3D::Play(const bool loop)
{
	if (!m_instance) { return; }
	SoundInstance::Play(loop);
}

//-----------------------------------------------------------------------------
// Name: SetPosition()
// Desc: エミッター座標設定
//-----------------------------------------------------------------------------
void SoundInstance3D::SetPosition(const Math::Vector3& pos)
{
	if (m_instance == nullptr) { return; }
	m_emitter.SetPosition(pos);
	m_instance->Apply3D(AUDIO.GetListener(), m_emitter, false);
}



//=============================================================================
// 
// AudioManager
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: AudioManager()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
AudioManager::AudioManager()
	: m_pAudioEngine(nullptr)
	, m_playList()
	, m_listener()
	, m_userVolume(0.2f)
	, m_peakLevels()
	, m_RMSLevels()
{
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 初期化
//-----------------------------------------------------------------------------
bool AudioManager::Initialize()
{
	//--------------------------------------------------
	// AudioEngine初期化
	//--------------------------------------------------
	// フラグ作成
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_ReverbUseFilters;
	// 生成
	m_pAudioEngine = std::make_unique<DirectX::AudioEngine>(eflags);
	m_pAudioEngine->SetReverb(DirectX::Reverb_Off);
	m_pAudioEngine->SetMasterVolume(m_userVolume);
	if (m_pAudioEngine == nullptr) {
		assert(0 && "[Initialize] : オーディオエンジン生成失敗");
		return false;
	}

	//--------------------------------------------------
	// リスナー初期化 ※Orient(オリエンタル ... 方向)
	//--------------------------------------------------
	m_listener.OrientFront = Math::Vector3(0.0f, 0.0f, 1.0f);

	return true;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: 解放
//-----------------------------------------------------------------------------
void AudioManager::Release()
{
	AllStop();
	m_pAudioEngine = nullptr;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void AudioManager::Update(const Math::Vector3& pos, const Math::Vector3& dir)
{
	if (m_pAudioEngine == nullptr) { return; }

	//--------------------------------------------------
	// オーディオエンジンの更新
	//--------------------------------------------------
	m_pAudioEngine->Update();

	//--------------------------------------------------
	// リスナー更新
	//--------------------------------------------------
	// 座標
	m_listener.SetPosition(pos);
	// 方向
	m_listener.OrientFront = dir;

	//--------------------------------------------------
	// 再生中のサウンドループ
	//--------------------------------------------------

	// 全リスト
	for (auto iter = m_playList.begin(); iter != m_playList.end();) {

		// フェード更新
		if (iter->second) { iter->second->UpdateFadeOut(); }

		// 再生中か確認
		if (!iter->second->IsPlaying()) {
			// リストから削除
			iter = m_playList.erase(iter);
			continue;
		}

		++iter;
	}
}

//-----------------------------------------------------------------------------
// Name: Play()
// Desc: 2Dサウンドを再生
//-----------------------------------------------------------------------------
bool AudioManager::Play(const std::string& filepath, const float volume, const bool loop)
{
	if (!m_pAudioEngine) { return false; }

	// サウンドエフェクト生成
	std::shared_ptr<SoundEffect> sound_data = KdResFac.GetSound(filepath);
	if (!sound_data) { return false; }

	// インスタンス生成
	std::shared_ptr<SoundInstance> instance = std::make_shared<SoundInstance>();
	if (instance) {
		instance->Initialize(sound_data);
		instance->SetVolume(volume);
		instance->Play(loop);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: Play3D()
// Desc: 3Dサウンドを再生
//-----------------------------------------------------------------------------
bool AudioManager::Play3D(const std::string& filepath, const Math::Vector3& pos, const float volume)
{
	if (!m_pAudioEngine) { return false; }

	// サウンドエフェクト生成
	std::shared_ptr<SoundEffect> sound_data = KdResFac.GetSound(filepath);
	if (!sound_data) { return false; }

	// 3Dインスタンス生成
	std::shared_ptr<SoundInstance3D> instance = std::make_shared<SoundInstance3D>();
	if (instance) {
		instance->Initialize(sound_data);
		instance->SetVolume(volume);
		instance->Play();
		instance->SetPosition(pos);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: AllStop()
// Desc: 再生リストの音楽を停止する
//-----------------------------------------------------------------------------
void AudioManager::AllStop()
{
	for (auto& sound : m_playList) {
		sound.second->Stop();
	}
}

//-----------------------------------------------------------------------------
// Name: AllFadeOut()
// Desc: 再生リストの音楽をフェードアウトさせる
//-----------------------------------------------------------------------------
void AudioManager::AllFadeOut()
{
	for (auto& sound : m_playList) {
		sound.second->SetFadeOut();
	}
}

//-----------------------------------------------------------------------------
// Name: GetSoundInstance()
// Desc: 2Dサウンドのインスタンスを所得
//-----------------------------------------------------------------------------
std::shared_ptr<SoundInstance> AudioManager::GetSoundInstance(const std::string& filepath)
{
	if (!m_pAudioEngine) { return false; }

	// サウンドエフェクト生成
	std::shared_ptr<SoundEffect> sound_data = KdResFac.GetSound(filepath);
	if (!sound_data) { return false; }

	// インスタンス生成
	std::shared_ptr<SoundInstance> instance = std::make_shared<SoundInstance>();
	if (instance) {
		instance->Initialize(sound_data);
		return instance;
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
// Name: GetSoundInstance3D()
// Desc: 3Dサウンドのインスタンスを所得
//-----------------------------------------------------------------------------
std::shared_ptr<SoundInstance3D> AudioManager::GetSoundInstance3D(const std::string& filepath, const Math::Vector3& pos)
{
	if (!m_pAudioEngine) { return false; }

	// サウンドエフェクト生成
	std::shared_ptr<SoundEffect> sound_data = KdResFac.GetSound(filepath);
	if (!sound_data) { return false; }

	// インスタンス生成
	std::shared_ptr<SoundInstance3D> instance = std::make_shared<SoundInstance3D>();
	if (instance) {
		instance->Initialize(sound_data);
		instance->SetPosition(pos);
		return instance;
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
// Name: SetUserSettingVolume()
// Desc: ユーザー設定の音量を設定
//-----------------------------------------------------------------------------
void AudioManager::SetUserSettingVolume(const float volume)
{
	if (m_pAudioEngine == nullptr) { return; }
	m_userVolume = volume;
	m_pAudioEngine->SetMasterVolume(m_userVolume);
}

//-----------------------------------------------------------------------------
// Name: AddplayList()
// Desc: 再生リストに登録
//-----------------------------------------------------------------------------
void AudioManager::AddplayList(const std::shared_ptr<SoundInstance>& sound)
{
	if (sound.get() == nullptr) { return; }
	// アドレスをUINTに変換して登録
	m_playList[(UINT)(sound.get())] = sound;
}

//-----------------------------------------------------------------------------
// Name: UpdateVolumeMeter()
// Desc: ボリュームメータ更新 ※修正.不具合
//-----------------------------------------------------------------------------
void AudioManager::UpdateVolumeMeter()
{
	if (m_pAudioEngine->GetMasterVoice() == nullptr) return;

	HRESULT result = S_OK;

	// ボリュームメータ(APO)作成
	IUnknown* pVolumeMeterAPO = nullptr;
	result = XAudio2CreateVolumeMeter(&pVolumeMeterAPO);

	// EFFECT_DESCRIPTOR の作成
	XAUDIO2_EFFECT_DESCRIPTOR descriptor = {};
	descriptor.InitialState = true;
	descriptor.OutputChannels = 2;
	descriptor.pEffect = pVolumeMeterAPO;

	// EFFECT_CHAIN の作成
	XAUDIO2_EFFECT_CHAIN chain = {};
	chain.EffectCount = 1;
	chain.pEffectDescriptors = &descriptor;
	result = m_pAudioEngine->GetMasterVoice()->SetEffectChain(&chain);

	// APO解放
	pVolumeMeterAPO->Release();

	// 受信用構造体 設定
	XAUDIO2FX_VOLUMEMETER_LEVELS Levels = {};
	//	Levels.pPeakLevels = m_peakLevels;
	//	Levels.pRMSLevels = m_RMSLevels;
	Levels.ChannelCount = 2;

	// パラメータ受信
	result = m_pAudioEngine->GetMasterVoice()->GetEffectParameters(0, &Levels, sizeof(Levels));
}