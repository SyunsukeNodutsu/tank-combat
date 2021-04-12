//-----------------------------------------------------------------------------
// File: KdResourceFactory.h
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class KdResourceFactory
// Desc: リソース管理クラス
//-----------------------------------------------------------------------------
class KdResourceFactory
{
public:

	// 管理を廃棄する
	void Clear();

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// インスタンスの所得
	static KdResourceFactory& GetInstance() { static KdResourceFactory instance; return instance; }
	// モデルデータ所得
	std::shared_ptr<KdModel> GetModel(const std::string& filepath);
	// テクスチャデータ所得
	std::shared_ptr<KdTexture> GetTexture(const std::string& filepath);
	// サウンドデータ所得
	std::shared_ptr<SoundEffect> GetSound(const std::string& filepath);
	// Json所得
	json11::Json GetJson(const std::string& filepath);

private:
	std::unordered_map<std::string, std::shared_ptr<KdModel>>		m_modelMap;	// 管理マップ.モデルデータ
	std::unordered_map<std::string, std::shared_ptr<KdTexture>>		m_texMap;	// 管理マップ.テクスチャ
	std::unordered_map<std::string, std::shared_ptr<SoundEffect>>	m_soundMap;	// 管理マップ.サウンドデータ
	std::unordered_map<std::string, json11::Json>					m_jsonMap;	// 管理マップ.Json

private:
	KdResourceFactory();
	json11::Json LoadJson(const std::string& filepath);
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define KdResFac KdResourceFactory::GetInstance()