//-----------------------------------------------------------------------------
// File: Button.h
//
// Edit: 2021/02/23 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//--------------------------------------------------
// Name: enum class ButtonState
// Desc: ボタンの状態 ※修正.離された際の状態も欲しい
//--------------------------------------------------
enum class ButtonState
{
	eNone,		// なにもない
	eEasing,	// イージングで出現中
	eChoice,	// ボタン上にマウスカーソルが存在
	ePush,		// ボタン上にマウスカーソルが存在 なおかつ押された
};

//-----------------------------------------------------------------------------
// Name: class SpriteButton
// Desc: 2Dのボタンクラス ※効果音を引数で設定できるようにしたほうがいい
//-----------------------------------------------------------------------------
class SpriteButton
{
public:

	// コンストラクタ
	SpriteButton();
	// 更新 ※選択し.マウスをクリックしたらtrue
	void Update(POINT mouse_pos);
	// 2D描画
	void DrawSprite();

	//--------------------------------------------------
	// 設定.所得
	//--------------------------------------------------
	// 初期設定
	void SetInfo(const std::array<std::string, 3> tex_paths, const Math::Vector2 pos, const Math::Vector2 size);
	// イージング設定
	void SetEasing(const EASING_TYPE type, const float time_length, const float start, const float end, const bool moveX = true);
	// 押されたか所得
	const bool IsPush() const { return (m_state == ButtonState::ePush); }
	// マウスカーソルがボタンの上に存在か所得
	const bool IsSelect() const { return  (m_state == ButtonState::eChoice); }

private:
	ButtonState						m_state;			// ボタンの状態
	ButtonState						m_prevState;		// 1フレーム前のボタンの状態
	Easing							m_easing;			// イージング
	std::shared_ptr<KdTexture>		m_texture_normal;	// テクスチャ.通常状態
	std::shared_ptr<KdTexture>		m_texture_choice;	// テクスチャ.選択状態
	std::shared_ptr<KdTexture>		m_texture_push;		// テクスチャ.押された状態
	Math::Vector2					m_position;			// 座標
	Math::Vector2					m_size;				// 矩形 ※(x...width), (y...height)
	bool							m_isPush;			// 押しているか
	bool							m_isEasing;			// イージングさせるか
	bool							m_isPushLButton;
	bool							m_isMoveX;			// イージングが縦横どっちか true...X(横)

private:
	const ButtonState CheckState(const Math::Vector2 center_pos, const Math::Vector2 mouse_pos);
};