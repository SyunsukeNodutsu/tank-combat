//-----------------------------------------------------------------------------
// File: Button.cpp
//
// Edit: 2021/02/23 野筒隼輔
//-----------------------------------------------------------------------------
#include "Button.h"

//-----------------------------------------------------------------------------
// Name: SpriteButton()
// Desc: コンストラクタ
//-----------------------------------------------------------------------------
SpriteButton::SpriteButton()
	: m_state(ButtonState::eNone)
	, m_prevState(ButtonState::eNone)
	, m_easing()
	, m_texture_normal(nullptr)
	, m_texture_choice(nullptr)
	, m_texture_push(nullptr)
	, m_position(Math::Vector2(0.0f, 0.0f))
	, m_size(Math::Vector2(0.0f, 0.0f))
	, m_isPush(false)
	, m_isEasing(false)
	, m_isPushLButton(false)
	, m_isMoveX(true)
{
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 更新
//-----------------------------------------------------------------------------
void SpriteButton::Update(POINT mouse_pos)
{
	//--------------------------------------------------
	// Easing更新
	//--------------------------------------------------
	if(m_isEasing) {
		// 更新
		m_easing.Update();
		// 移動量所得
		if (m_isMoveX) { m_position.x = m_easing.GetMovement(); }
		else { m_position.y = m_easing.GetMovement(); }
		// 完了確認
		m_isEasing = m_easing.IsEmit();
	}

	//--------------------------------------------------
	// ボタン選択の確認
	//--------------------------------------------------

	// マウス座標をfloatに変換
	Math::Vector2 mousePos = {};
	mousePos.x = static_cast<float>(mouse_pos.x);
	mousePos.y = static_cast<float>(mouse_pos.y);

	// SpriteShaderは中心基準なので 左上基準に修正
	constexpr float height = 720.0f;
	constexpr float width = 1280.0f;
	const Math::Vector2 tex_pos = Math::Vector2(m_position.x + (width * 0.5f), m_position.y * -1 + (height * 0.5f));

	// ボタン状態確認
	if (!m_isEasing) {// 出現(Easing)が終わってから
		m_state = CheckState(tex_pos, mousePos);
	}

	//--------------------------------------------------
	// 効果音
	//--------------------------------------------------

	// 1フレーム前のボタン状態と比較
	if (m_state != m_prevState) {
		// 選択時の効果音
		if (m_state == ButtonState::eChoice) {
			AUDIO.Play("Data/Audio/SE/Select.wav");
		}
		// 選択 -> 押した時の効果音
		if (m_state == ButtonState::ePush) {
			AUDIO.Play("Data/Audio/SE/Enter.wav", 0.5f);
		}
	}

	// 1フレーム前の状態更新
	m_prevState = m_state;
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D描画
//-----------------------------------------------------------------------------
void SpriteButton::DrawSprite()
{
	// 状態に合わせてテクスチャを確定
	KdTexture* texture = nullptr;
	switch (m_state)
	{
	case ButtonState::eNone:
	case ButtonState::eEasing:	texture = m_texture_normal.get();	break;
	case ButtonState::eChoice:	texture = m_texture_choice.get();	break;
	case ButtonState::ePush:	texture = m_texture_push.get();		break;
	}

	// 描画
	KdMatrix matrix = {};
	matrix.CreateTranslation(m_position.x, m_position.y, 0.0f);
	SHADER.m_spriteShader.SetMatrix(matrix);
	SHADER.m_spriteShader.DrawTex(texture, 0, 0);
}

//-----------------------------------------------------------------------------
// Name: SetInfo()
// Desc: 情報設定 ※初期設定
//-----------------------------------------------------------------------------
void SpriteButton::SetInfo(const std::array<std::string, 3> tex_paths, const Math::Vector2 pos, const Math::Vector2 size)
{
	if (tex_paths.size() != 3) { assert(0 && "[] : error set texture"); }

	m_texture_normal	= KdResFac.GetTexture(tex_paths[0]);
	m_texture_choice	= KdResFac.GetTexture(tex_paths[1]);
	m_texture_push		= KdResFac.GetTexture(tex_paths[2]);
	m_position			= pos;
	m_size				= size;
}

//-----------------------------------------------------------------------------
// Name: SetEasing()
// Desc: Easingの設定
//-----------------------------------------------------------------------------
void SpriteButton::SetEasing(const EASING_TYPE type, const float time_length, const float start, const float end, const bool moveX)
{
	m_easing.Emit(type, time_length, start, end);
	m_isEasing = true;
	m_isMoveX = moveX;// 縦横どちらにEasingするか
	// 初期座標
	if (m_isMoveX) { m_position.x = start; }
	else { m_position.y = start; }
}

//-----------------------------------------------------------------------------
// Name: CheckState()
// Desc: ボタンの状態を確認
//-----------------------------------------------------------------------------
const ButtonState SpriteButton::CheckState(const Math::Vector2 center_pos, const Math::Vector2 mouse_pos)
{
	// マウスカーソルがボタンの上に存在
	if (mouse_pos.x <= center_pos.x + (m_size.x * 0.5f) && mouse_pos.x >= center_pos.x - (m_size.x * 0.5f) &&
		mouse_pos.y <= center_pos.y + (m_size.y * 0.5f) && mouse_pos.y >= center_pos.y - (m_size.y * 0.5f)) {
		// マウス左ボタン入力 ※修正.ボタンを設定できるようにする
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
			// 押したままドラッグした際に入力されてしまうのを防止
			if (m_prevState == ButtonState::eChoice) {
				return ButtonState::ePush;
			}
		}
		else {
			return ButtonState::eChoice;
		}
	}

	return ButtonState::eNone;
}