//-----------------------------------------------------------------------------
// File: KdAnimation.h
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
struct KdAnimKeyQuaternion;
struct KdAnimKeyVector3;
struct KdAnimationData;

//-----------------------------------------------------------------------------
// Name: class KdAnimator
// Desc:
//-----------------------------------------------------------------------------
class KdAnimator
{
public:

	// コンストラクタ
	KdAnimator();
	// アニメーションの登録
	inline void SetAnimation(std::shared_ptr<KdAnimationData>& data, bool loop = true);
	// アニメーションが終了しているか
	bool IsAnimationEnd() const;

	// アニメーションの更新
	//・nodeVector	... ノード一覧
	//・speed		... アニメーション速度
	//・brendWeight	... 重み
	void AdvanceTime(std::vector<KdModel::Node>& nodeVector, float speed = 1.0f, float brendWeight = 1.0f);

private:
	std::shared_ptr<KdAnimationData>	m_spAnimation;	// アニメーションデータ
	float								m_time;			//
	bool								m_isLoop;		// ループするか
};

//-----------------------------------------------------------------------------
// Name: class KdBrendAnimator
// Desc: キーフレームによる補完をさらに強化
//-----------------------------------------------------------------------------
class KdBrendAnimator
{
public:

	// アニメーションのブレンド更新
	void AdvanceTime(std::vector<KdModel::Node>& node, float speed = 1.0f);
	// アニメーションの登録
	void SetAnimation(std::shared_ptr<KdAnimationData>& data, float changeSec = 0.5f, bool loop = true);
	// アニメーションが終了しているか
	bool IsAnimationEnd() const;

private:
	std::unique_ptr<KdAnimator> m_pNowAnimator;		// 現在再生中のアニメーション
	std::unique_ptr<KdAnimator> m_pNextAnimator;	// 次に遷移するアニメーション
	CommonTimer					m_brendTimer;		// ブレンド時間管理用タイマー
};

//-----------------------------------------------------------------------------
// Name: struct KdAnimKeyQuaternion
// Desc: アニメーションキー(クォータニオン)
//-----------------------------------------------------------------------------
struct KdAnimKeyQuaternion
{
	float			m_time = 0.0f;	// 時間
	KdQuaternion	m_quat;			// クォータニオンデータ
};

//-----------------------------------------------------------------------------
// Name: struct KdAnimKeyVector3
// Desc: アニメーションキー(ベクトル)
//-----------------------------------------------------------------------------
struct KdAnimKeyVector3
{
	float			m_time = 0.0f;	// 時間
	KdVector3		m_vec;			// 3Dベクトルデータ
};

//-----------------------------------------------------------------------------
// Name: struct KdAnimationData
// Desc: アニメーションデータ
//-----------------------------------------------------------------------------
struct KdAnimationData
{
	// アニメーション名
	std::string m_name;

	// アニメの長さ
	float m_maxLength = 0.0f;

	// 1ノードのアニメーションデータ
	struct Node
	{
		void Interpolate(KdMatrix& aDst, float aTime);
		bool InterpolateTranslation(KdVector3& result, float time);
		bool InterpolateRotation(KdQuaternion& result, float time);

		// 対象モデルのノードのOffset値
		int m_nodeOffset = -1;

		// 各チャンネル
		std::vector<KdAnimKeyVector3>		m_translations;	// 位置
		std::vector<KdAnimKeyQuaternion>	m_rotations;	// 回転
	};

	// 全ノード用アニメーションデータ
	std::vector<Node> m_nodes;
};