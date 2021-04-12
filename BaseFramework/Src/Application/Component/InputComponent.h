//-----------------------------------------------------------------------------
// File: InputComponent.h
//
// Edit: 2021/03/10 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once
#include "Application/Game/GameObject.h"

//-----------------------------------------------------------------------------
// Name: namespace Input
// Desc: ボタンとスティックを再現
//-----------------------------------------------------------------------------
namespace Input
{
	// スティック
	enum Axes
	{
		L,
		R,
		AXS_MAX,
	};

	// ボタン
	enum Button
	{
		A,
		B,
		X,
		Y,
		L1,
		R1,
		BTN_MAX,
	};

	// ボタンの状態
	enum ButtonState
	{
		FREE	= 0,		// 何もしてない
		ENTER	= 1,		// 押したとき
		STAY	= 1 << 1,	// 押し続けられた
		EXIT	= 1 << 2,	// 離したとき
	};
}

//-----------------------------------------------------------------------------
// Name: class InputComponent
// Desc: 入力管理クラス
//-----------------------------------------------------------------------------
class InputComponent
{
public:

	// コンストラクタ
	InputComponent(GameObject& aOwner);
	// デストラクタ
	virtual ~InputComponent() {}
	// 更新
	virtual void Update() {}
	// ボタン押し
	void PushButton(Input::Button no);
	// ボタン離し
	void ReleaseButton(Input::Button no);

	// 軸所得
	inline const Math::Vector2& GetAxis(Input::Axes no) const
	{
		assert(no != Input::Axes::AXS_MAX);
		return m_axes[no];
	}

	// ボタン所得
	inline int GetButton(Input::Button no) const
	{
		assert(no != Input::Button::BTN_MAX);
		return m_buttons[no];
	}

protected:
	GameObject										m_owner;	// 持ち主.オーナー
	std::array<Math::Vector2, Input::Axes::AXS_MAX> m_axes;		// 操作軸 配列array<型,要素数>
	std::array<int, Input::Button::BTN_MAX>			m_buttons;	// ボタン
};

//-----------------------------------------------------------------------------
// Name: class BasicInputComponent
// Desc: 基本のコントローラ ※各プロセスで使用
//-----------------------------------------------------------------------------
class BasicInputComponent : public InputComponent
{
public:

	// コンストラクタ
	BasicInputComponent(GameObject& owner);
	// 更新
	void Update() override;

	//--------------------------------------------------
	// 所得
	//--------------------------------------------------
	// マウス座標所得
	const Math::Vector2 GetMousePos();

private:
	POINT m_mousePosition;
};

//-----------------------------------------------------------------------------
// Name: class TankInputComponent
// Desc: 戦車用コントローラ
//-----------------------------------------------------------------------------
class TankInputComponent : public InputComponent
{
public:

	// コンストラクタ
	TankInputComponent(GameObject& owner);
	// 更新
	void Update() override;

private:
	POINT m_mousePosition;
	POINT m_prevMousePosition;
};