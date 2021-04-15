//-----------------------------------------------------------------------------
// File: GameObject.h
//
// Edit: 2021/02/11 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
class	CameraComponent;	// カメラ
class	CollisionComponent;	// 衝突判定
class	InputComponent;		// コントローラ
class	ModelComponent;		// モデル

enum	OBJECT_TAG;			// 識別用タグ

//-----------------------------------------------------------------------------
// Name: class GameObject
// Desc: ゲームオブジェクト基底クラス this_ptrからshared_ptr変換可能
//-----------------------------------------------------------------------------
class GameObject : public std::enable_shared_from_this<GameObject>
{
public:

	// コンストラクタ
	GameObject();
	// デストラクタ
	virtual ~GameObject();
	// 初期化
	virtual void Deserialize(const json11::Json& json_object);
	// 更新
	virtual void Update() {}
	// ImGui更新
	virtual void ImGuiUpdate();
	// 描画
	virtual void Draw();
	// シャドウマップ描画
	virtual void DrawShadowMap();
	// 半透明物描画
	virtual void DrawEffect() {}
	// 2D描画
	virtual void DrawSprite() {}

	//--------------------------------------------------
	// 設定
	//--------------------------------------------------
	// 行列の設定
	inline void SetMatrix(const KdMatrix& aMat) { m_worldMatrix = aMat; }
	// m_tagに引数の値を設定
	inline void SetTag(UINT aTag) { m_tag = aTag; }
	// m_isAliveフラグのOFF 死亡させる
	virtual void Destroy() { m_isAlive = false; }
	// 消滅させる
	virtual void Exclusion() { m_isEnable = false; }

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// ワールド行列の所得
	inline const KdMatrix& GetMatrix() const { return m_worldMatrix; }
	// 1フレーム前の行列を所得
	inline const KdMatrix& GetPrevMatrix() { return m_prevMatrix; }
	// m_isAliveの所得
	inline bool IsAlive() const { return m_isAlive; }
	// m_isEnableの所得 ※リストから除外するかを調べる
	inline bool IsEnable() const { return m_isEnable; }
	// タグの所得
	inline UINT GetTag() { return m_tag; }
	// クラスの名前の所得
	inline const char* GetClasName() const { return m_className.c_str(); }
	// 名前の所得
	inline const char* GetName() const { return m_name.c_str(); }
	// 3Dモデルの名前の所得
	inline const char* GetModelName() const { return m_modelName.c_str(); }
	// prefabのファイルパス所得
	inline const char* GetPrefabFilepath() const { return m_prefabFilepath.c_str(); }
	// 動いた分の行列を所得 ※1フレーム前の逆行列をワールド行列と合成
	KdMatrix GetOneMove();

	//--------------------------------------------------
	// 所得.コンポーネント
	//--------------------------------------------------
	std::shared_ptr<CameraComponent> GetCameraComponent() { return m_spCameraComponent; }
	std::shared_ptr<CollisionComponent> GetCollisionComponent() { return m_spCollisionComponent; }
	std::shared_ptr<ModelComponent> GetModelComponent() { return m_spDrawModelComponent; }
	std::shared_ptr<ModelComponent> GetCollModelComponent() { return m_spCollisionModelComponent; }

protected:
	KdMatrix							m_worldMatrix;				// ワールド行列
	KdMatrix							m_prevMatrix;				// 1フレーム前の行列
	std::string							m_className;				// クラスの名前
	std::string							m_name;						// 名前
	std::string							m_modelName;				// 3Dモデルの名前
	std::string							m_collisionModelName;		// 衝突判定用モデルの名前
	std::string							m_prefabFilepath;			// プレハブの名前
	UINT								m_tag;						// 識別タグ
	float								m_colRadius;				// 球判定に使用する半径
	bool								m_isAlive;					// 生存しているかのFlg
	bool								m_isEnable;					// リストから除外かのFlg

	std::shared_ptr<CameraComponent>	m_spCameraComponent;		// コンポーネント.カメラ
	std::shared_ptr<CollisionComponent>	m_spCollisionComponent;		// コンポーネント.衝突判定
	std::shared_ptr<InputComponent>		m_spInputComponent;			// コンポーネント.入力
	std::shared_ptr<ModelComponent>		m_spDrawModelComponent;		// コンポーネント.描画用モデル
	std::shared_ptr<ModelComponent>		m_spCollisionModelComponent;// コンポーネント.衝突判定用モデル

private:
	void Release();
	void TransformSetting(const json11::Json& json_object);
	void ParameterSetting(const json11::Json& json_object);
};

//--------------------------------------------------
// Name; CreateGameObject()
// Desc: クラス名からGameObjectを生成
//--------------------------------------------------
std::shared_ptr<GameObject> CreateGameObject(const std::string& class_name);

//--------------------------------------------------
// Name: enum OBJECT_TAG
// Desc: 識別タグ
//--------------------------------------------------
enum OBJECT_TAG
{
	TAG_None			= 0,		// 属性なし(初期設定)

	TAG_Player			= 1,		// 操作キャラ属性
	TAG_Allies			= 1 << 1,	// 操作していない味方属性
	TAG_Enemy			= 1 << 2,	// 敵属性

	TAG_Tank			= TAG_Player | TAG_Allies | TAG_Enemy,// 戦車

	TAG_StageObject		= 1 << 3,	// 地面
	TAG_TreeObject		= 1 << 4,	// 木
	TAG_HouseObject		= 1 << 5,	// 家
	TAG_GrassObject		= 1 << 6,	// 草垣
};