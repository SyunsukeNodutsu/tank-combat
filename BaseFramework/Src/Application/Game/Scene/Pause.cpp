//-----------------------------------------------------------------------------
// File: Pause.cpp
//
// Edit: 2021/03/18 �쓛����
//-----------------------------------------------------------------------------
#include "Pause.h"
#include "Scene.h"
#include "Application/main.h"
#include "Application/Manager/ClearManager.h"

//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: �R���X�g���N�^
//-----------------------------------------------------------------------------
Pause::Pause()
	: m_spState(nullptr)
	, m_spBackTexture(nullptr)
	, m_tmpVolume(0.0f)
	, m_isEnable(false)
	, m_isPlaying(false)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: ������
//-----------------------------------------------------------------------------
void Pause::Deserialize()
{
	LoadTexture();

	// ���.���ߏ�Ԃ���
	m_spState = std::make_shared<StateCommand>();
	if (m_spState) { m_spState->Deserialize(*this); }
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: �X�V
//-----------------------------------------------------------------------------
void Pause::Update()
{
	if (SCENE.IsEdiorMode()) { return; }

	// �؂�ւ��m�F ���L�������ɂ�����炸�m�F
	CheckChangeEnable();

	if (!m_isEnable) { return; }

	// �}�E�X���W����
	/*
	* �C��
	* GetDesktopMousePos
	* GetClientMousePos �݂����Ȋ����ŃN���X�ɂ܂Ƃ߂��ق�������
	*/
	POINT mouse_pos = {};
	GetCursorPos(&mouse_pos);
	ScreenToClient(APP.m_window.GetWndHandle(), &mouse_pos);

	// ��Ԃ��Ƃ̍X�V
	if (m_spState) { m_spState->Update(*this, mouse_pos); }
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D�`��
//-----------------------------------------------------------------------------
void Pause::DrawSprite()
{
	if (!m_isEnable) { return; }

	// �w�i
	if (m_spBackTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spBackTexture.get(), 0, 0);
	}

	// �ړI
	if (m_spMissionTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spMissionTexture.get(), 360, 20);
	}

	// ����
	if (m_spTipsTexture) {
		SHADER.m_spriteShader.SetMatrix(KdMatrix());
		SHADER.m_spriteShader.DrawTex(m_spTipsTexture.get(), 0, -300);
	}

	// ��Ԃ��Ƃ�2D�`��
	if (m_spState) { m_spState->DrawSprite(*this); }
}

//-----------------------------------------------------------------------------
// Name: CheckChangeEnable()
// Desc: �|�[�Y��ʐ؂�ւ��̊m�F
//-----------------------------------------------------------------------------
void Pause::CheckChangeEnable()
{
	// �Q�[�����̂�
	if (!m_isPlaying) { return; }
	// �Q�[�����I�����Ă��邩
	if (CLEAR_MAGER.IsGameEnd()) { return; }
	// �A�N�e�B�u���m�F
	if (!(APP.m_window.GetWinInfo().dwWindowStatus & WS_ACTIVECAPTION)) { return; }

	static bool isPush = true;
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
		if (!isPush) {
			isPush = !isPush;
			m_isEnable = !m_isEnable;
			SCENE.SetCursorShow(m_isEnable);

			// ���ʂ������� ���C��.AUDIO�Ɏ���
			if (m_isEnable) {
				m_tmpVolume = AUDIO.GetUserSettingVolume();
				AUDIO.SetUserSettingVolume(AUDIO.GetUserSettingVolume() * 0.4f);
				// 
				if (m_spState) { m_spState->Deserialize(*this); }
			}
			// ���ʂ����ɖ߂�
			else {
				AUDIO.SetUserSettingVolume(m_tmpVolume);

				// �}�E�X���W�𒆉��ɖ߂��Ă���ĊJ ... �}�ɃJ�����������̂�\�h(�C��.�N���X�ɂ܂Ƃ߂�)
				{
					// ��~��������W
					RECT rect = {};
					SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
					const int stopX = (rect.right - rect.left) / 2;
					const int stopY = (rect.bottom - rect.top) / 2;

					// ��ʊO�ɏo�Ȃ��悤�ɌŒ�
					SetCursorPos(stopX, stopY);
				}
			}
		}
	}
	else if (isPush) { isPush = !isPush; }
}

//-----------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: �e�N�X�`���ǂݍ���
//-----------------------------------------------------------------------------
void Pause::LoadTexture()
{
	m_spBackTexture		= KdResFac.GetTexture("Data/Texture/Pause/Back.png");
	m_spMissionTexture	= KdResFac.GetTexture("Data/Texture/Pause/Mission/Stage01.png.png");
	m_spTipsTexture		= KdResFac.GetTexture("Data/Texture/Pause/Tips/Order.png");
}



//=============================================================================
// 
// StateCommand
// 
//=============================================================================

//-----------------------------------------------------------------------------
// Name: StateCommand()
// Desc: �R���X�g���N�^
//-----------------------------------------------------------------------------
Pause::StateCommand::StateCommand()
	: m_spState(nullptr)
	, m_buttonArray()
	, m_stateOrder(true)
{
}

//-----------------------------------------------------------------------------
// Name: Deserialize()
// Desc: ������
//-----------------------------------------------------------------------------
void Pause::StateCommand::Deserialize(Pause& owner)
{
	m_spState = std::make_shared<StateOrder>();
	if (m_spState) { m_spState->Initialize(owner); }

	m_stateOrder = true;

	std::array<std::string, 3> tex_paths;
	tex_paths.fill("");

	tex_paths = {
		"Data/Texture/Pause/Button/OrderButton_N.png",
		"Data/Texture/Pause/Button/OrderButton_C.png",
		"Data/Texture/Pause/Button/OrderButton_C.png",
	};
	m_buttonArray[0].SetInfo(tex_paths, Math::Vector2(-540.0f, 310.0f), Math::Vector2(166.0f, 78.0f));

	tex_paths = {
		"Data/Texture/Pause/Button/SystemButton_N.png",
		"Data/Texture/Pause/Button/SystemButton_C.png",
		"Data/Texture/Pause/Button/SystemButton_C.png",
	};
	m_buttonArray[1].SetInfo(tex_paths, Math::Vector2(-360.0f, 310.0f), Math::Vector2(166.0f, 78.0f));
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: �X�V
//-----------------------------------------------------------------------------
void Pause::StateCommand::Update(Pause& owner, const POINT mouse_pos)
{
	// ��Ԃ��Ƃ̍X�V
	if (m_spState) { m_spState->Update(owner, mouse_pos); }

	//--------------------------------------------------
	// �e��X�V
	//--------------------------------------------------
	for (auto& button : m_buttonArray) {
		button.Update(mouse_pos);
	}

	//--------------------------------------------------
	// �I�����ꂽ�ۂ̏���
	//--------------------------------------------------
	// Order
	if (m_buttonArray[0].IsSelect()) {
		// �O�t���[���̏�Ԃ��r���ĕω����Ă�����
		if (!m_stateOrder) {
			m_spState = std::make_shared<StateOrder>();
			m_spState->Initialize(owner);
			m_stateOrder = true;
		}
	}
	// System
	if (m_buttonArray[1].IsSelect()) {
		if (m_stateOrder) {
			m_spState = std::make_shared<StateSystem>();
			m_spState->Initialize(owner);
			m_stateOrder = false;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: 2D�`��
//-----------------------------------------------------------------------------
void Pause::StateCommand::DrawSprite(Pause& owner)
{
	// ��Ԃ��Ƃ�2D�`��
	if (m_spState) { m_spState->DrawSprite(owner); }

	// �{�^��2D�`��
	for (auto& button : m_buttonArray) {
		button.DrawSprite();
	}
}