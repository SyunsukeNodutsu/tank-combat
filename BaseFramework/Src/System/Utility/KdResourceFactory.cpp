//-----------------------------------------------------------------------------
// File: KdResourceFactory.cpp
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#include "KdResourceFactory.h"

//-----------------------------------------------------------------------------
// Name: KdResourceFactory()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
KdResourceFactory::KdResourceFactory()
	: m_modelMap()
	, m_texMap()
	, m_soundMap()
	, m_jsonMap()
{
}

//-----------------------------------------------------------------------------
// Name: Clear()
// Desc: 管理マップのクリア
//-----------------------------------------------------------------------------
void KdResourceFactory::Clear()
{
	m_modelMap.clear();
	m_texMap.clear();
	m_soundMap.clear();
	m_jsonMap.clear();
}

//-----------------------------------------------------------------------------
// Name: GetModel()
// Desc: モデルデータ所得
//-----------------------------------------------------------------------------
std::shared_ptr<KdModel> KdResourceFactory::GetModel(const std::string& filepath)
{
	// 検索
	auto itFound = m_modelMap.find(filepath);

	// 初回
	if (itFound == m_modelMap.end())
	{
		// 生成
		auto newModel = std::make_shared<KdModel>();
		if (!newModel->Load(filepath)) {
			assert(0 && "[GetModel] : model notfound");
			return nullptr;
		}

		// リスト(map)に登録
		m_modelMap[filepath] = newModel;
		// 完了
		return newModel;
	}
	else {
		// 次回以降はすでに読み込んだデータ
		return (*itFound).second;
	}
}

//-----------------------------------------------------------------------------
// Name: GetTexture()
// Desc: テクスチャデータ所得
//-----------------------------------------------------------------------------
std::shared_ptr<KdTexture> KdResourceFactory::GetTexture(const std::string& filepath)
{
	// データ検索
	auto itFound = m_texMap.find(filepath);

	if (itFound == m_texMap.end())
	{
		// 生成
		auto newTexture = std::make_shared<KdTexture>();
		if (!(newTexture->Load(filepath))) {
			assert(0 && "[GetTexture] : notfound texture");
			return nullptr;
		}

		// リストに登録
		m_texMap[filepath] = newTexture;
		// 完了
		return newTexture;
	}
	else {
		// 次回以降はすでに読み込んだデータ
		return (*itFound).second;
	}
}

//-----------------------------------------------------------------------------
// Name: GetSound()
// Desc: サウンドデータ所得
//-----------------------------------------------------------------------------
std::shared_ptr<SoundEffect> KdResourceFactory::GetSound(const std::string& filepath)
{
	// 検索
	auto itFound = m_soundMap.find(filepath);

	// 初回
	if (itFound == m_soundMap.end())
	{
		// wstring変換
		std::wstring wFilepath = sjis_to_wide(filepath);

		// 生成
		auto newSound = std::make_shared<SoundEffect>();
		if (!newSound->Load(filepath)){
			assert(0 && "[GetSound] : notfound soundfile");
			return nullptr;
		}

		// リストに登録
		m_soundMap[filepath] = newSound;
		// 完了
		return newSound;
	}
	else {
		// 次回以降はすでに読み込んだデータ
		return (*itFound).second;
	}
}

//-----------------------------------------------------------------------------
// Name: GetJson()
// Desc: Jsonデータの所得
//-----------------------------------------------------------------------------
json11::Json KdResourceFactory::GetJson(const std::string& filepath)
{
	// 検索
	auto itFound = m_jsonMap.find(filepath);

	// 初回
	if (itFound == m_jsonMap.end())
	{
		// Jsonファイル読み込み
		json11::Json json = LoadJson(filepath);
		if (json.is_null()) {
			assert(0 && "[GetJson] : notfound jsonfile");
			return nullptr;
		}

		// リストに登録
		m_jsonMap[filepath] = json;
		// 完了
		return json;
	}
	else {
		// 次回以降はすでに読み込んだデータ
		return (*itFound).second;
	}
}

//-----------------------------------------------------------------------------
// Name: LoadJson()
// Desc: Jsonファイルの読み込み
//-----------------------------------------------------------------------------
json11::Json KdResourceFactory::LoadJson(const std::string& filepath)
{
	// jsonファイルを開く
	std::fstream ifs(filepath);
	if (ifs.fail()) {
		assert(0 && "[LoadJson] : error json filepath");
		return nullptr;
	}

	// 文字列として読み込み
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	// 文字列のJSONを解析(パース)する
	std::string err = {};
	json11::Json jsonObj = json11::Json::parse(strJson, err);
	if (err.size() > 0) {
		assert(0 && "[LoadJSON] : error json parse");
		return nullptr;
	}

	// 完了
	return jsonObj;
}