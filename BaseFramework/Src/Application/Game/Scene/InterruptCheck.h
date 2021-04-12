//-----------------------------------------------------------------------------
// File: InterruptCheck.h
//
// Edit: 2021/03/21 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class InterruptCheck
// Desc: 割り込みの確認管理クラス ※ゲーム終了の確認など
//-----------------------------------------------------------------------------
class InterruptCheck
{
public:

	// 初期化
	void Deserialize();
	// 更新
	void Update();
	// 描画
	void DrawSprite();

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// インスタンスの所得
	static InterruptCheck& GetInstance() { static InterruptCheck instance; return instance; }
	// 確認
	const bool IsEnable() const { return m_isEnable; }

private:
	std::array<SpriteButton, 2> m_buttonArray;		// ボタン
	std::shared_ptr<KdTexture>	m_checkExitTexture;	// 確認の枠テクスチャ
	bool						m_isEnable;			// 有効かどうか

private:
	InterruptCheck();
};

//-----------------------------------------------------------------------------
// Define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define IRQ InterruptCheck::GetInstance()// IRQ ... 割り込み要求 (Interrupt Request)