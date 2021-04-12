//-----------------------------------------------------------------------------
// File: ResultProcess.h
//
// Edit: 2021/03/27 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//--------------------------------------------------
// 前方宣言
//--------------------------------------------------
enum CLEAR_LANK;
class Tank;

//-----------------------------------------------------------------------------
// Name: class ResultProcess
// Desc: リザルトの管理
//-----------------------------------------------------------------------------
class ResultProcess : public GameObject
{
public:

	// コンストラクタ
	ResultProcess();
	// 初期化
	void Deserialize(const json11::Json& json_object) override;
	// 更新
	void Update() override;
	// 2D描画
	void DrawSprite() override;

private:
	CLEAR_LANK							m_clearLank;			// クリアのランク
	std::shared_ptr<Easing>				m_spEasing;				// 文字列のイージング
	float								m_resultTexX;			// 結果テクスチャのX座標
	float								m_textureAlpha;			// テクスチャの明度
	int									m_seCount;				// SEを発生させる際のフレームカウンタ
	bool								m_isMissionComplete;	// ゲームをクリアしたかどうか

	// テクスチャ
	std::shared_ptr<KdTexture>	m_spBackTexture;		// 背景のテクスチャ
	std::shared_ptr<KdTexture>	m_spKeyTipsTexture;		// 入力のヒントのテクスチャ
	std::shared_ptr<KdTexture>	m_spResultOKTexture;	// 結果.成功
	std::shared_ptr<KdTexture>	m_spResultNOTexture;	// 結果.失敗
	std::shared_ptr<KdTexture>	m_spAliveTexture;		// 生存
	std::shared_ptr<KdTexture>	m_spDeadTexture;		// 行動不能
	// スコアの数値(0～9)
	std::array<std::shared_ptr<KdTexture>, 10>	m_spNumberTextureArray;
	// クリアランク(S.A.B.Cの順番で合計4)
	std::array<std::shared_ptr<KdTexture>, 4>	m_spLankTextureArray;

private:
	void LoadTexture();
	void UpdateEasing();
	void DrawSpriteResult();
	std::tuple<uint8_t, uint8_t, uint8_t> CheckHitlate(uint8_t attakNum, uint8_t hitNum);
	const CLEAR_LANK CheckClearLank() const;
	void SaveResult(const std::string filepath);
};

//--------------------------------------------------
// Name: enum CLEAR_LANK
// Desc: クリアランク Sが最高 Dが最低
//--------------------------------------------------
enum CLEAR_LANK { eS, eA, eB, eC, };