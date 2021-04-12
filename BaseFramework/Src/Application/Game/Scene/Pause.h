//-----------------------------------------------------------------------------
// File: Pause.h
//
// Edit: 2021/03/18 �쓛����
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class Pause
// Desc: �|�[�Y(�ꎞ��~)�Ǘ��N���X
//-----------------------------------------------------------------------------
class Pause
{
	class StateBase;
public:

	// ������
	void Deserialize();
	// �X�V
	void Update();
	// �`��
	void DrawSprite();

	//--------------------------------------------------
	// ����
	//--------------------------------------------------
	// �C���X�^���X�̏���
	static Pause& GetInstance() { static Pause instance; return instance; }
	// �L�����m�F
	const bool IsEnable() const { return m_isEnable; }
	// �Q�[�������ǂ����m�F
	const bool IsPlaying() const { return m_isPlaying; }

	//--------------------------------------------------
	// �ݒ�
	//--------------------------------------------------
	void SetGamePlaying(const bool playing) { m_isPlaying = playing; }

private:
	std::shared_ptr<StateBase>	m_spState;			// ���݂̏��
	std::shared_ptr<KdTexture>	m_spBackTexture;	// �w�i�̃e�N�X�`��
	std::shared_ptr<KdTexture>	m_spMissionTexture;	// �ړI�̃e�N�X�`��
	std::shared_ptr<KdTexture>	m_spTipsTexture;	// �����̃e�N�X�`��
	float						m_tmpVolume;		// �ꎞ�I�ɉ��ʂ�ۑ�
	bool						m_isEnable;			// �L�����ǂ���(�ꎞ��~�����ǂ���)
	bool						m_isPlaying;		// �Q�[�����ǂ���

private:
	Pause();
	void CheckChangeEnable();
	void LoadTexture();

private:
	//--------------------------------------------------
	// ��Ԃ��ƂɃX�e�[�g��
	//--------------------------------------------------

	// ���ۃX�e�[�g�x�[�X
	class StateBase
	{
	public:
		// ������
		virtual void Deserialize(Pause& owner) = 0;
		// �X�V
		virtual void Update(Pause& owner, const POINT mouse_pos) = 0;
		// 2D�`��
		virtual void DrawSprite(Pause& owner) = 0;
	};

	// �R�}���h�I��ҋ@���
	class StateCommand : public StateBase
	{
		class StateBase;
	public:
		// �R���X�g���N�^
		StateCommand();
		// ������
		void Deserialize(Pause& owner) override;
		// �X�V
		void Update(Pause& owner, const POINT mouse_pos) override;
		// 2D�`��
		void DrawSprite(Pause& owner) override;

	private:
		std::shared_ptr<StateBase>	m_spState;		// ���(�K�w�X�e�[�g�}�V��)
		std::array<SpriteButton, 2>	m_buttonArray;	// �{�^��
		bool						m_stateOrder;	// ���ꖳ�����Ȏ���

	private:

		// �R�}���h��ԊǗ� ���ۃC���^�[�t�F�[�X
		class StateBase
		{
		public:
			virtual void Initialize(Pause& owner) = 0;
			virtual void Update(Pause& owner, const POINT& mouse_pos) = 0;
			virtual void DrawSprite(Pause& owner) = 0;
		};

		// ���߃{�^����I��
		class StateOrder : public StateBase
		{
		public:
			StateOrder();
			void Initialize(Pause& owner) override;
			void Update(Pause& owner, const POINT& mouse_pos) override;
			void DrawSprite(Pause& owner) override;
		private:
			std::array<SpriteButton, 2>	m_buttonArray;	// �{�^��
			int							m_frameCount;	// �t���[���̃J�E���^
		};

		// �V�X�e���{�^����I��
		class StateSystem : public StateBase
		{
		public:
			StateSystem();
			void Initialize(Pause& owner) override;
			void Update(Pause& owner, const POINT& mouse_pos) override;
			void DrawSprite(Pause& owner) override;
		private:
			std::array<SpriteButton, 3>	m_buttonArray;	// �{�^��
			int							m_frameCount;	// �t���[���̃J�E���^
		};
	};
};

//-----------------------------------------------------------------------------
// Define: �ȒP�ɃA�N�Z�X
//-----------------------------------------------------------------------------
#define PAUSE Pause::GetInstance()