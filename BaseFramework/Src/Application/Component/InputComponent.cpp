//-----------------------------------------------------------------------------
// File: InputComponent.cpp
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#include "InputComponent.h"
#include "Application/main.h"
#include "Application/Game/Scene/Scene.h"
#include "Application/Manager/DebugManager.h"

//=============================================================================
// 
// InputComponent
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: InputComponent()
// Desc: 
//-----------------------------------------------------------------------------
InputComponent::InputComponent(GameObject& owner)
	: m_owner(owner)
{
	// 軸.ボタンを初期化
	for (auto& axis : m_axes) { axis = { 0.0f, 0.0f }; }
	m_buttons.fill(Input::ButtonState::FREE);
}

//-----------------------------------------------------------------------------
// Name: PushButton()
// Desc: 
//-----------------------------------------------------------------------------
void InputComponent::PushButton(Input::Button no)
{
	assert(no != Input::Button::BTN_MAX);

	// 押している(bit演算)
	if (m_buttons[no] & Input::ButtonState::STAY)
	{
		// ENTER反転とAND ... ENTERだけをOFF
		m_buttons[no] &= ~Input::ButtonState::ENTER;
	}
	// 押していない
	else
	{
		// ENTERをOR ... ENTERをON
		m_buttons[no] |= Input::ButtonState::ENTER;
		// STAYをOR  ... STAYをON
		m_buttons[no] |= Input::ButtonState::STAY;
	}
}

//-----------------------------------------------------------------------------
// Name: ReleaseButton()
// Desc: 
//-----------------------------------------------------------------------------
void InputComponent::ReleaseButton(Input::Button no)
{
	assert(no != Input::Button::BTN_MAX);

	// 押している(bit演算)
	if (m_buttons[no] & Input::ButtonState::STAY)
	{
		// ENTER反転とAND ... ENTERだけをOFF
		m_buttons[no] &= ~Input::ButtonState::ENTER;
		// STAY反転とAND  ... STAYだけをOFF
		m_buttons[no] &= ~Input::ButtonState::STAY;
		// EXITをOR		  ... EXITをON
		m_buttons[no] |= Input::ButtonState::EXIT;
	}
	// 押していない
	else
	{
		// EXIT反転とAND ... EXITだけをOFF
		m_buttons[no] &= ~Input::ButtonState::EXIT;
	}
}


//=============================================================================
// 
// BasicInputComponent
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BasicInputComponent::BasicInputComponent(GameObject& owner)
	: InputComponent(owner)
	, m_mousePosition({ 0, 0 })
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void BasicInputComponent::Update()
{
	//--------------------------------------------------
	// マウス情報
	//--------------------------------------------------

	// 現在の座標所得
	GetCursorPos(&m_mousePosition);

	//--------------------------------------------------
	// ボタン入力
	//--------------------------------------------------

	// マウスの左ボタン ... L1
	if (GetAsyncKeyState(VK_LBUTTON)) { PushButton(Input::Button::L1); }
	else { ReleaseButton(Input::Button::L1); }

	// マウスの右ボタン ... R1
	if (GetAsyncKeyState(VK_RBUTTON)) { PushButton(Input::Button::R1); }
	else { ReleaseButton(Input::Button::R1); }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
const Math::Vector2 BasicInputComponent::GetMousePos()
{
	const float mouseX = static_cast<float>(m_mousePosition.x);
	const float mouseY = static_cast<float>(m_mousePosition.y);
	return Math::Vector2(mouseX, mouseY);
}



//=============================================================================
// 
// TankInputComponent
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
TankInputComponent::TankInputComponent(GameObject& owner)
	: InputComponent(owner)
	, m_mousePosition({ 0, 0 })
	, m_prevMousePosition({ 0, 0 })
{
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void TankInputComponent::Update()
{
	// アクティブか確認
	if (!(APP.m_window.GetWinInfo().dwWindowStatus & WS_ACTIVECAPTION)) { return; }

	for (auto& axis : m_axes) {
		axis = { 0.0f,0.0f };
	}

	//--------------------------------------------------
	// 左軸の入力 ※WASD
	//--------------------------------------------------
	if (GetAsyncKeyState('W') & 0x8000) { m_axes[Input::Axes::L].y = 1.0f; }
	if (GetAsyncKeyState('A') & 0x8000) { m_axes[Input::Axes::L].x = -1.0f; }
	if (GetAsyncKeyState('S') & 0x8000) { m_axes[Input::Axes::L].y = -1.0f; }
	if (GetAsyncKeyState('D') & 0x8000) { m_axes[Input::Axes::L].x = 1.0f; }

	//--------------------------------------------------
	// 右軸の入力 ※マウス
	//--------------------------------------------------

	// 停止させる座標
	RECT rect = {};
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	const int stopX = (rect.right - rect.left) / 2;
	const int stopY = (rect.bottom - rect.top) / 2;

	// 現在の座標所得
	GetCursorPos(&m_mousePosition);

	// 画面外に出ないように固定
	SetCursorPos(stopX, stopY);

	// スティック操作
	m_axes[Input::R].x = static_cast<float>(m_mousePosition.x - m_prevMousePosition.x);
	m_axes[Input::R].y = static_cast<float>(m_mousePosition.y - m_prevMousePosition.y);

	// 今フレームの座標記憶
	m_prevMousePosition = POINT{ stopX, stopY };

	//--------------------------------------------------
	// ボタン入力
	//--------------------------------------------------

	// マウスの左ボタン ... L1
	if (GetAsyncKeyState(VK_LBUTTON)) { PushButton(Input::Button::L1); }
	else { ReleaseButton(Input::Button::L1); }

	// マウスの右ボタン ... R1
	if (GetAsyncKeyState(VK_RBUTTON)) { PushButton(Input::Button::R1); }
	else { ReleaseButton(Input::Button::R1); }
}